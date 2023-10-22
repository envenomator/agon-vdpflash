#include "fabgl.h"
#include "zdi.h"
#include "cpu.h"
#include "flashloader.h"
#include "eZ80F92.h"
#include <esp_task_wdt.h>
#include "serial.h"
#include "updater.h"

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
    terminal.write("Agon MOS ZDI flash utility - version 0.71\r\n\r\n");
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

void ask_proceed(void) {
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
    
    // setup serial
    setupSerial2();

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
    
    // configure SPI
    cpu->instruction_out (SPI_CTL, 0x04);
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
        address += PAGESIZE;
        buffer += PAGESIZE;
        filesize -= PAGESIZE;
    }
    zdi->write_memory(address, filesize, buffer);
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
    terminal.write("    named as \"MOS.bin\"\r\n");
    terminal.write("\r\nPress ENTER to proceed:");
    waitforKey(0x0D);
    terminal.write("\r\n\r\n");
}

void loop() {
    uint32_t page,pages,filesize,readsize;
    uint16_t productid;
    uint8_t  revision;
    uint8_t memval;
    char buffer[128];
    serialpackage_t status;
    
    //boot_screen();
    //ask_initial();

    while(Serial2.available()) {
        Serial2.read(); // read terminal startup 0x17/0x00/0x80/0x17 sequences
    }

    boot_screen();
    terminal.write("Action                          Status\r\n");
    terminal.write("--------------------------------------\r\n");
    terminal.write("Checking ZDI interface        - ");
    productid = zdi->get_productid();
    revision = zdi->get_revision();
    if((productid != 7)) {
        fg_red();
        terminal.write("DOWN - check cabling and reset");
        while(1);
    }
    sprintf(buffer,"UP (ID %X.%02X)\r\n",productid, revision);
    terminal.write(buffer);

    terminal.write("Uploading flashloader to ez80 - ");

    init_ez80();
    ZDI_upload();

    terminal.write("Done\r\n");

    cpu->pc(USERLOAD);
    cpu->setContinue(); // start uploaded program
    
    terminal.write("Starting flashloader          - ");

    status = getStatus();
    if((status.state == 'S') && (status.status == 1))
        terminal.write("Done\r\n");
    else {
        fg_red();
        sprintf(buffer, "Status <%d> <%d> <0x%08X> - ",status.state, status.status, status.result);
        terminal.write(buffer);
        terminal.write("Error");
        while(1);
    }

    terminal.write("Opening MOS.bin from SD card  - ");
    status = getStatus();    
    if((status.state == 'F') && (status.status == 1))
        terminal.write("Done");
    else {
        fg_red();
        terminal.write("Error opening \"MOS.bin\"");
        while(1);
    }
    filesize = status.result;
    if(filesize > 0x20000) {
        fg_red();
        terminal.write(" Invalid file size for \"MOS.bin\"");
        while(1);
    }
    sprintf(buffer, " (%d bytes)\r\n", filesize);
    terminal.write(buffer);
    
    terminal.write("Reading to ez80 memory        - ");
    status = getStatus();
    if((status.state == 'M') && (status.status == 1))
        terminal.write("Done");
    else {
        fg_red();
        terminal.write(" Error reading file into memory");
        while(1);
    }
    readsize = status.result;
    if(readsize == filesize) terminal.write("\r\n");
    else {
        fg_red();
        terminal.write(" Error reading file");
        while(1);
    }

    // DEBUG
    //status = getStatus();
    //terminal.write("\r\n");
    //sprintf(buffer, "State <%d><%02x><'%c'> - status <0x%02x> - value <0x%08X>\r\n", status.state, status.state, status.state, status.status, status.result);
    //terminal.write(buffer);
    //while(1);
    // DEBUG

    status = getStatus();
    sprintf(buffer, "Receiving %d bytes", status.result);
    terminal.write(buffer);
    receiveFirmware(&terminal, status.result);
    while(1);
    /*
    // TEMP
    status = getStatus();
    sprintf(buffer, "Receiving %d bytes", status.result);
    terminal.write(buffer);
    uint32_t total = 0;
    int count = 0;
    for(uint32_t n = 0; n < status.result; n++) {
        while(!Serial2.available());
        total += Serial2.read();
        count++;
        if(count > 20480) {
            count = 0;
            terminal.write('.');
        }
    }
    status = getStatus();
    sprintf(buffer, "\r\nReceived total: %d - local total: %d\r\n", status.result, total);
    terminal.write(buffer);
    //if(total == status.result) terminal.write("Match");
    //else terminal.write("No match");
    // TEMP
    */

    ask_proceed();
    Serial2.write(1);

    terminal.write("Erasing flash                 - ");
    status = getStatus();
    if((status.state == 'E') && (status.status == 1))
        terminal.write("Done\r\n");
    else {
        fg_red();
        terminal.write("Error erasing flash");
        while(1);
    }
    if(status.result != 128) {
        fg_red();
        sprintf(buffer, "Error (%d pages erased)", status.result);
        terminal.write(buffer);
        while(1);
    }

    // determine number of pages to write
    pages = filesize/1024;
    if(filesize%1024) pages += 1;
    terminal.write("\r\nProgramming ");
    bool done = false;
    while(!done) {
        status = getStatus();
        if(status.state == 'X') {
            terminal.write(".");
        }
        else done = true;
    }
    if((status.state == 'P') && (status.status == 1))
        terminal.write(" Done\r\n");
    else {
        fg_red();
        terminal.write("\r\nError writing to flash");
        while(1);
    }
    if(status.result != pages) {
        fg_red();
        sprintf(buffer, "\r\nError (%d pages written)", status.result);
        terminal.write(buffer);
        while(1);
    }
    terminal.write("\r\n");
    terminal.write("MOS has been programmed to ez80 flash\r\n");
    terminal.write("Please (re)program ESP32 with matching VDP");

    while(1);
}
