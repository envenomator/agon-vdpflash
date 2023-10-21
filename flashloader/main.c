#include <ez80.h>
#include "src_fatfs\ff.h"
#include "spi.h"
#include "defines.h"
#include <stdint.h>
#include "agontimer.h"
#include "protocol.h"

#define PAGESIZE	1024
#define FLASHPAGES	128
#define FLASHSTART	0x0
#define BAUDRATE	500000

#define MOSFILENAME	"MOS.bin"
#define LOADADDRESS	0x50000

extern void init_UART0(void);

extern void enableFlashKeyRegister(void);
extern void lockFlashKeyRegister(void);
extern void fastmemcpy(UINT24 destination, UINT24 source, UINT24 size);
extern void reset(void);

FATFS 	fs;

int main(int argc, char * argv[]) {
	FRESULT	fr;
	FIL	   	fil;
	UINT24  br;	
	void * 	dest;
	UINT24 size;
	
	UINT24	counter,pagemax, lastpagebytes;
	UINT24 addressto,addressfrom;
	UINT8	value;
	UINT24 timer;
	init_spi();
	init_UART0();

	/*
	while(1) {
		putch('A');
		delayms(500);
		putch('X');
		delayms(500);
	}
	*/
	while(1) {
		sendStatus('S', 1, 0xAABBCCDD);
		delayms(500);
		sendStatus('E', 1, 0);
		delayms(500);
	}




	f_mount(&fs, "", 1);

	addressto = FLASHSTART;
	addressfrom = LOADADDRESS;

	fr = f_open(&fil, MOSFILENAME, FA_READ);
	if(fr == FR_OK) {

		di();
		size = f_size(&fil);
		//waitZDI(FEEDBACK_OPEN, size);

		fr = f_read(&fil, (void *)LOADADDRESS, size, &br);

		f_close(&fil);
		//waitZDI(FEEDBACK_FILEDONE, br);
		
		// Wait for user to acknowledge proceed (remote ZDI)
		//waitZDI(FEEDBACK_PROCEED, 0);
		
		// Unprotect and erase flash
		enableFlashKeyRegister();	// unlock Flash Key Register, so we can write to the Flash Write/Erase protection registers
		FLASH_PROT = 0;				// disable protection on all 8x16KB blocks in the flash
		enableFlashKeyRegister();	// will need to unlock again after previous write to the flash protection register
		FLASH_FDIV = 0x5F;			// Ceiling(18Mhz * 5,1us) = 95, or 0x5F

		// Erase all flash pages
		for(counter = 0; counter < FLASHPAGES; counter++)
		{
			FLASH_PAGE = counter;
			FLASH_PGCTL = 0x02;			// Page erase bit enable, start erase

			do
			{
				value = FLASH_PGCTL;
			}
			while(value & 0x02);// wait for completion of erase			
		}
		
		// determine number of pages to write
		pagemax = size/PAGESIZE;
		if(size%PAGESIZE) // last page has less than PAGESIZE bytes
		{
			pagemax += 1;
			lastpagebytes = size%PAGESIZE;			
		}
		else lastpagebytes = PAGESIZE; // normal last page

		//waitZDI(FEEDBACK_ERASEDONE, pagemax);

		// write out each page to flash
		for(counter = 0; counter < pagemax; counter++)
		{
			if(counter == (pagemax - 1)) // last page to write - might need to write less than PAGESIZE
				fastmemcpy(addressto,addressfrom,lastpagebytes);				
			else 
				fastmemcpy(addressto,addressfrom,PAGESIZE);
		
			addressto += PAGESIZE;
			addressfrom += PAGESIZE;
			//timer = 0;
			//while(timer++ < 32768);
			////waitZDI(FEEDBACK_PAGEWRITTEN, counter);
		}
		lockFlashKeyRegister();	// lock the flash before WARM reset
		//waitZDI(FEEDBACK_PAGEWRITTEN, counter);
	}
	else //waitZDI(FEEDBACK_OPEN, 0);
	while(1);
	return 0;
}

