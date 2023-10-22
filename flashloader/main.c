#include <ez80.h>
#include "src_fatfs\ff.h"
#include "spi.h"
#include "defines.h"
#include <stdint.h>
#include "agontimer.h"
#include "protocol.h"

#define PAGESIZE	1024
#define BUFFERSIZE	1024
#define FLASHPAGES	128
#define FLASHSTART	0x0
#define BAUDRATE	500000

#define MOSFILENAME	"MOS.bin"
#define VDPFILENAME "VDP.bin"
#define LOADADDRESS	0x50000

extern void init_UART0(void);

extern void enableFlashKeyRegister(void);
extern void lockFlashKeyRegister(void);
extern void fastmemcpy(UINT24 destination, UINT24 source, UINT24 size);
extern void reset(void);

FATFS 	fs;

int main(int argc, char * argv[]) {
	FRESULT	frmos,frvdp;
	FIL	   	filmos,filvdp;
	UINT24  br;	
	void * 	dest;
	UINT24  mossize, vdpsize;
	
	UINT24	counter,pagemax, lastpagebytes;
	UINT24 addressto,addressfrom;
	UINT8	value;
	UINT24 timer;

	unsigned char buffer[BUFFERSIZE];
	int n;
	uint32_t checksum;

	init_spi();
	init_UART0();

	f_mount(&fs, "", 1);

	sendStatus('S', 1, 0); // startup complete

	addressto = FLASHSTART;
	addressfrom = LOADADDRESS;

	frmos= f_open(&filmos, MOSFILENAME, FA_READ);
	if(frmos != FR_OK) {
		sendStatus('F', 0, frmos); // file not read ok
		while(1);
	}
	frvdp= f_open(&filvdp, VDPFILENAME, FA_READ);
	if(frvdp != FR_OK) {
		sendStatus('V', 0, frvdp); // file not read ok
		while(1);
	}
	mossize = f_size(&filmos);
	sendStatus('F', 1, mossize); // file read ok
	vdpsize = f_size(&filvdp);
	sendStatus('V', 1, vdpsize); // file read ok

	frmos= f_read(&filmos, (void *)LOADADDRESS, mossize, &br);
	sendStatus('M', 1, br); // file read into memory

	f_close(&filmos);

	// Wait for user to acknowledge to proceed
	getch();
	
	// Unprotect and erase flash
	enableFlashKeyRegister();	// unlock Flash Key Register, so we can write to the Flash Write/Erase protection registers
	FLASH_PROT = 0;				// disable protection on all 8x16KB blocks in the flash
	enableFlashKeyRegister();	// will need to unlock again after previous write to the flash protection register
	FLASH_FDIV = 0x5F;			// Ceiling(18Mhz * 5,1us) = 95, or 0x5F

	// Mass erase flash
	FLASH_PGCTL = 0x01;	// mass erase bit enable, start erase all pages
	do {
		value = FLASH_PGCTL;
	} while(value & 0x01); // wait for completion of erase
	sendStatus('E', 1, 128);

	// determine number of pages to write
	pagemax = mossize/PAGESIZE;
	if(mossize%PAGESIZE) // last page has less than PAGESIZE bytes
	{
		pagemax += 1;
		lastpagebytes = mossize%PAGESIZE;			
	}
	else lastpagebytes = PAGESIZE; // normal last page

	// write out each page to flash
	for(counter = 0; counter < pagemax; counter++)
	{
		if(counter == (pagemax - 1)) // last page to write - might need to write less than PAGESIZE
			fastmemcpy(addressto,addressfrom,lastpagebytes);				
		else 
			fastmemcpy(addressto,addressfrom,PAGESIZE);
	
		addressto += PAGESIZE;
		addressfrom += PAGESIZE;
		sendStatus('X', 1, counter+1);
	}
	lockFlashKeyRegister();	// lock the flash before WARM reset
	sendStatus('P', 1, counter); // programming ok

	getch(); // sync to receiver
	// Sending VDP update
	checksum = 0;
	while(1) {
		frvdp= f_read(&filvdp, (void *)buffer, BUFFERSIZE, &br);
		if(br == 0) break;
		for(n = 0; n < br; n++) {
			putch(buffer[n]);
			checksum += buffer[n];
		}
	}
	sendStatus('W', 1, checksum);
	while(1);
	return 0;
}

