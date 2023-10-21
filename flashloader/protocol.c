#include "protocol.h"

//#define UART_LSR_TEMT	0x40
// Blocking non-interrupt putch to UART0
int putch(int c)
{
	while((UART0_LSR & UART_LSR_TEMT) == 0);
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

UCHAR read_UART0(CHAR *pData, int *nbytes) {
	UCHAR lsr ;
	UCHAR status = UART_ERR_NONE ;
	int   index = 0 ;

	while( UART_ERR_NONE == status ) {
		lsr = UART0_LSR ;											//! Read the line status.
		
		if( 0 != (lsr & UART_LSR_BREAKINDICATIONERR) ) {			//! Check if there is any Break Indication Error.
			status = UART_ERR_BREAKINDICATIONERR ;					//! Failure code.
		}
		if( 0 != (lsr & UART_LSR_FRAMINGERR) ) {					//! Check if there is any Framing error.
			status = UART_ERR_FRAMINGERR ;							//! Failure code.
		}
		if( 0 != (lsr & UART_LSR_PARITYERR) ) {						//! Check if there is any Parity error.
			status = UART_ERR_PARITYERR ;							//! Failure code.
		}
		if( 0 != (lsr & UART_LSR_OVERRRUNERR) )	{					//! Check if there is any Overrun error.
			status = UART_ERR_OVERRUNERR ;							//! Failure code.
		}
		if( 0 != (lsr & UART_LSR_DATA_READY) ) {					//! See if there is any data byte to be read.
			pData[ index++ ] = UART0_RBR ;							//! Read it from the receive buffer register.
		}
		if( index == (*nbytes) ) {									//! On completion break the while loop.
			break ;
		}
	}
	*nbytes = index ;
	return status ;
	
}

int getch(void) {
	CHAR ch;
	int nbytes = 1;
	UCHAR status = read_UART0(&ch, &nbytes);
	nbytes = (UINT)ch;
	nbytes = (nbytes & 0x0000FF);
	return (UART_ERR_NONE!=status) ? -1 : nbytes;
}
