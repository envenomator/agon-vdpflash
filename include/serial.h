#ifndef SERIAL_H
#define SERIAL_H

#include <HardwareSerial.h>

#define UART_BR					1152000	// Max baud rate; previous stable value was 384000
#define UART_NA					-1
#define UART_TX					2
#define UART_RX					34
#define UART_RTS				13		// The ESP32 RTS pin (eZ80 CTS)
#define UART_CTS	 			14		// The ESP32 CTS pin (eZ80 RTS)
#define UART_RX_SIZE			256		// The RX buffer size
#define UART_RX_THRESH			128		// Point at which RTS is toggled

void setupSerial2(void);

typedef struct {
	char		state;	// current state at ez80
	uint8_t		status;	// 0: failure, 1: success
	uint32_t	result;	// result code
} serialpackage_t;

serialpackage_t getStatus(void);

#endif