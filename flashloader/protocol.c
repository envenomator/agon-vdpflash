#include "protocol.h"

// Blocking non-interrupt putch to UART0
int putch(int c)
{
	//UINT8 lsr,temt;
	
	while((UART0_LSR & 0x40) == 0);
	UART0_THR = c;
	return c;
}

void sendStatus(char state, UINT8 status, UINT32 result) {
	putch(state);
	putch(status);
	putch((result) & 0xFF);
	putch((result >> 8) & 0xFF);
	putch((result >> 16) & 0xFF);
	putch((result >> 24) & 0xFF);
}
