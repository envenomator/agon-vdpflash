#include "serial.h"

void setupSerial2(void) {
	Serial2.end();
	Serial2.setRxBufferSize(UART_RX_SIZE);					// Can't be called when running
	Serial2.begin(UART_BR, SERIAL_8N1, UART_RX, UART_TX);
	Serial2.setHwFlowCtrlMode(HW_FLOWCTRL_RTS, 64);			// Can be called whenever
	Serial2.setPins(UART_NA, UART_NA, UART_CTS, UART_RTS);	// Must be called after begin
}

serialpackage_t getStatus(void) {
	serialpackage_t status;

	while(!Serial2.available());
	status.state = Serial2.read();
	status.status = Serial2.read();
	status.result = Serial2.read();
	status.result |= (Serial2.read() << 8);
	status.result |= (Serial2.read() << 16);
	status.result |= (Serial2.read() << 24);
	return status;
}
