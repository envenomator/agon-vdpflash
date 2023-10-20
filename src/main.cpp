#include "fabgl.h"
#include "zdi.h"
#include "cpu.h"
#include "flashloader.h"
#include "eZ80F92.h"
#include <esp_task_wdt.h>

#define PAGESIZE       1024
#define USERLOAD    0x40000
#define BREAKPOINT  0x40020

#define ZDI_TCKPIN 26
#define ZDI_TDIPIN 27

fabgl::PS2Controller    PS2Controller;
fabgl::VGA16Controller  DisplayController;
fabgl::Terminal         terminal;

CPU*                    cpu;
ZDI*                    zdi;

void term_printf (const char* format, ...) {
	va_list list;
   	int size = vsnprintf(nullptr, 0, format, list) + 1;
    //char buffer[size + 1];
    char buffer[256];
   	va_start(list, format);
   	if (size > 0) {
    	va_end(list);
     	va_start(list, format);
     	vsnprintf(buffer, size, format, list);
        terminal.write (buffer, size);
   	}
   	va_end(list);
}

uint32_t waitcontinueLoader(void) {
    uint32_t result, pc;

    while(cpu->isRunning()) delay(100); // wait for ez80 to hit breakpoint
    cpu->setBreak();
    pc = cpu->pc();
    cpu->bc(0x100); // write '1' to register B, don't care about 'C'
    result = cpu->hl();
    cpu->pc(pc);
    cpu->setContinue();
    return result;
}

void fg_white(void) {
    terminal.write("\e[44;37m"); // background: blue, foreground: white
}
void fg_red(void) {
    terminal.write("\e[41;37m"); // background: blue, foreground: red
}
void boot_screen() {
    // initialize terminal
    fg_white();
    terminal.write("\e[2J");     // clear screen
    terminal.write("\e[1;1H");   // move cursor to 1,1
    terminal.write("Agon MOS ZDI flash utility - version 0.6\r\n\r\n");
}

void waitforKey(uint8_t key) {
    fabgl::Keyboard *kb = PS2Controller.keyboard();
    fabgl::VirtualKeyItem item;
    
    while(1) {
        if(kb->getNextVirtualKey(&item, 0)) {
            if(item.ASCII == key) break;
        }
    }
}

void ask_initial() {
    terminal.write("This utility will (baremetal) program a new MOS to the Agon ez80 flash,\r\n");
    terminal.write("from a file on the SD card. This can be performed in case of\r\n");
    terminal.write("an initially erased or corrupted flash, or having previously flashed\r\n");
    terminal.write("an incompatible MOS version.\r\n");
    terminal.write("\r\nRequirements before proceeding:\r\n");
    terminal.write(" 1) Connect two cables between the GPIO and ZDI ports:\r\n");
    terminal.write("    - ESP GPIO26 to ZDI TCK (pin 4)\r\n");
    terminal.write("    - ESP GPIO27 to ZDI TDI (pin 6)\r\n");
    terminal.write(" 2) Place the required MOS version on the SD card's root directory\r\n"); 
    terminal.write("    with filename \"MOS.bin\"\r\n");
    terminal.write(" 3) Reset the board after inserting the SD card\r\n\r\n");
    terminal.write("Press ENTER to proceed:");
    waitforKey(0x0D);
    terminal.write("\r\n\r\n");
}

void ask_proceed() {
    terminal.write("\r\nPress [y] to start programming MOS:");
    waitforKey('y');
    terminal.write("\r\n\r\n");
}

void setup() {
    // Disable the watchdog timers
    disableCore0WDT(); delay(200);								

    esp_task_wdt_init(30, false); // in case WDT cannot be removed

    // setup keyboard/PS2
    PS2Controller.begin(PS2Preset::KeyboardPort0, KbdMode::CreateVirtualKeysQueue);

    // setup VGA display
    DisplayController.begin();
    DisplayController.setResolution(VGA_640x480_60Hz);
    
    // setup terminal
    terminal.begin(&DisplayController);
    terminal.enableCursor(true);

    // setup ZDI interface
    zdi = new ZDI(ZDI_TCKPIN, ZDI_TDIPIN);
    cpu = new CPU(zdi);
}

void init_ez80(void) {
    cpu->setBreak();
    cpu->setADLmode(true);
    cpu->instruction_di();  
    
    // configure default GPIO
    cpu->instruction_out (PB_DDR, 0xff);
    cpu->instruction_out (PC_DDR, 0xff);
    cpu->instruction_out (PD_DDR, 0xff);
    
    cpu->instruction_out (PB_ALT1, 0x0);
    cpu->instruction_out (PC_ALT1, 0x0);
    cpu->instruction_out (PD_ALT1, 0x0);
    cpu->instruction_out (PB_ALT2, 0x0);
    cpu->instruction_out (PC_ALT2, 0x0);
    cpu->instruction_out (PD_ALT2, 0x0);

    cpu->instruction_out (TMR0_CTL, 0x0);
    cpu->instruction_out (TMR1_CTL, 0x0);
    cpu->instruction_out (TMR2_CTL, 0x0);
    cpu->instruction_out (TMR3_CTL, 0x0);
    cpu->instruction_out (TMR4_CTL, 0x0);
    cpu->instruction_out (TMR5_CTL, 0x0);

    cpu->instruction_out (UART0_IER, 0x0);
    cpu->instruction_out (UART1_IER, 0x0);

    cpu->instruction_out (I2C_CTL, 0x0);
    cpu->instruction_out (FLASH_IRQ, 0x0);

    cpu->instruction_out (SPI_CTL, 0x4);

    cpu->instruction_out (RTC_CTRL, 0x0);
    
    // configure internal flash
    cpu->instruction_out (FLASH_ADDR_U,0x00);
    cpu->instruction_out (FLASH_CTRL,0b00101000);   // flash enabled, 1 wait state
    
    // configure internal RAM chip-select range
    cpu->instruction_out (RAM_ADDR_U,0xb7);         // configure internal RAM chip-select range
    cpu->instruction_out (RAM_CTL,0b10000000);      // enable
    // configure external RAM chip-select range
    cpu->instruction_out (CS0_LBR,0x04);            // lower boundary
    cpu->instruction_out (CS0_UBR,0x0b);            // upper boundary
    cpu->instruction_out (CS0_BMC,0b00000001);      // 1 wait-state, ez80 mode
    cpu->instruction_out (CS0_CTL,0b00001000);      // memory chip select, cs0 enabled

    // configure external RAM chip-select range
    cpu->instruction_out (CS1_CTL,0x00);            // memory chip select, cs1 disabled
    // configure external RAM chip-select range
    cpu->instruction_out (CS2_CTL,0x00);            // memory chip select, cs2 disabled
    // configure external RAM chip-select range
    cpu->instruction_out (CS3_CTL,0x00);            // memory chip select, cs3 disabled

    // set stack pointer
    cpu->sp(0x0BFFFF);
    // set program counter
    cpu->pc(0x000000);
}

void ZDI_upload(void) {
    uint32_t address,filesize;
    unsigned char *buffer;

    address = USERLOAD;
    buffer = &flashloader_bin[0];
    filesize = flashloader_bin_len;

    while(filesize > PAGESIZE) {
        zdi->write_memory(address, PAGESIZE, buffer);
        terminal.write(".");
        address += PAGESIZE;
        buffer += PAGESIZE;
        filesize -= PAGESIZE;
        sys_delay_ms(1);
    }
    zdi->write_memory(address, filesize, buffer);
}

void loop() {
    uint32_t page,pages,filesize,readsize;
    uint16_t productid;
    uint8_t memval;
    char buffer[128];

    boot_screen();
    ask_initial();
 
    boot_screen();
    terminal.write("Action                          Status\r\n");
    terminal.write("--------------------------------------\r\n");
    terminal.write("Checking ZDI interface        - ");
    productid = zdi->get_productid();
    if((productid != 7)) {
        fg_red();
        terminal.write("DOWN - check cabling and reset");
        while(1);
    }
    //terminal.write("UP (ID %X.%02X)\r\n",productid, zdi->get_revision());
    terminal.write("UP (");
    terminal.write(")\r\n");
    terminal.write("Uploading flashloader to ez80 - ");

    init_ez80();
    ZDI_upload();

    terminal.write(" Done\r\n");

    cpu->pc(USERLOAD);
    cpu->setBreakpoint(0, BREAKPOINT);
    cpu->enableBreakpoint(0);
    cpu->setContinue(); // start uploaded program
    
    terminal.write("Starting flashloader          - ");
    waitcontinueLoader();
    terminal.write("Done\r\n");   
    terminal.write("Opening file from SD card     - ");
    filesize = waitcontinueLoader();
    if(filesize == 0) {
        fg_red();
        terminal.write("Error opening \"MOS.bin\"");
        while(1);
    }
    if(filesize == 0xFFFFFF) {
        fg_red();
        terminal.write("Invalid file size for \"MOS.bin\"");
        while(1);
    }
    terminal.write("Done");
    //terminal.write(" (%d bytes)\r\n", filesize);
    terminal.write("(");
    sprintf(buffer, "%ld", filesize);
    terminal.write(buffer);
    terminal.write(")\r\n");
    
    terminal.write("Reading file to ez80 memory   - ");
    readsize = waitcontinueLoader();
    if(filesize == readsize) terminal.write("Done\r\n");
    else {
        fg_red();
        terminal.write("Error (");
        sprintf(buffer,"%ld", readsize);
        terminal.write(buffer);
        terminal.write(") bytes read\r\n");
        while(1);
    }
    ask_proceed();
    waitcontinueLoader();

    terminal.write("Erasing flash                 - ");
    delay(5000);
    pages = waitcontinueLoader();
    //waitcontinueLoader();
    // determine number of pages to write
    //pages = filesize/1024;
    //if(filesize%1024) pages += 1;

    sprintf(buffer, "%ld", pages);
    terminal.write(buffer);

    terminal.write(" - ");


    //terminal.write("%x-",(pages >> 24)&0xFF);
    //terminal.write("%x-",(pages >> 16)&0xFF);
    //terminal.write("%x-",(pages >> 8)&0xFF);
    //terminal.write("%x ",(pages)&0xFF);

    terminal.write("Done\r\n");

    terminal.write("Programming ");

    // Temp wait
    delay(10000);
    //page = 0;
    //do {
    //    page++;
    //    terminal.write(".");
    //    waitcontinueLoader();
    //
    //}
    //while(page < pages);

    terminal.write(" - ");

    pages = waitcontinueLoader();
    sprintf(buffer, "%ld pages written - ", pages);
    terminal.write(buffer);


    terminal.write(" Done\r\n\r\n");
    terminal.write("MOS has been programmed to ez80 flash\r\n");
    terminal.write("Please (re)program ESP32 with matching VDP");

    while(1);
}
