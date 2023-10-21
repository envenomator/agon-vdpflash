#include "serial.h"

void setupSerial2(void) {
	Serial2.end();
	Serial2.setRxBufferSize(UART_RX_SIZE);					// Can't be called when running
	Serial2.begin(UART_BR, SERIAL_8N1, UART_RX, UART_TX);
	Serial2.setHwFlowCtrlMode(HW_FLOWCTRL_RTS, 64);			// Can be called whenever
	Serial2.setPins(UART_NA, UART_NA, UART_CTS, UART_RTS);	// Must be called after begin
}