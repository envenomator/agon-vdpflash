#include "updater.h"
#include "fabgl.h"
#include "serial.h"

void receiveFirmware(fabgl::Terminal *terminal, uint32_t update_size) {
	char textbuffer[128];
	serialpackage_t status;
	esp_err_t err;
	esp_ota_handle_t update_handle = 0 ;
	const esp_partition_t *update_partition = NULL;
	const esp_partition_t *configured = esp_ota_get_boot_partition();
	const esp_partition_t *running = esp_ota_get_running_partition();

	update_partition = esp_ota_get_next_update_partition(NULL);
	err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
	if (err != ESP_OK) {
		sprintf(textbuffer, "esp_ota_begin failed, error=%d\n\r", err);
		terminal->write(textbuffer);
		while(1);
	}

	uint32_t remaining_bytes = update_size;
	const size_t buffer_size = 1024;

	uint32_t total = 0;
	int tickcount = 0;
	uint8_t input;

	while(remaining_bytes > 0) {

		size_t bytes_to_read = buffer_size;

		if(remaining_bytes < buffer_size) bytes_to_read = remaining_bytes;

		uint8_t buffer[buffer_size];
		
		for(size_t n = 0; n < bytes_to_read; n++) {
			while(!Serial2.available());
			input = Serial2.read();
			buffer[n] = input;
			total += input;
			tickcount++;
			if(tickcount > 20480) {
				tickcount = 0;
				terminal->write('.');
			}
		}

		err = esp_ota_write( update_handle, (const void *)buffer, bytes_to_read);
		if (err != ESP_OK) {
			sprintf(textbuffer, "esp_ota_write failed, error=%d", err);
			terminal->write(textbuffer);
			while(1);
		}

		remaining_bytes -= bytes_to_read;
	}

	status = getStatus();
    if(total == status.result) terminal->write("Checksum ok!\r\n");
    else {
		terminal->write("Checksum error!\r\n");
		while(1);
	}
	// checksum check
	//uint8_t checksum_complement = readByte_b();
	//printFmt("checksum_complement: 0x%x\n\r", checksum_complement);
	//if(uint8_t(code + checksum_complement)) {
	//	printFmt("checksum error!\n\r");
	//	return;
	//}
	//printFmt("checksum ok!\n\r");

	err = esp_ota_set_boot_partition(update_partition);
	if (err != ESP_OK) {
		terminal->write("esp_ota_set_boot_partition failed!");
		while(1);
	}

	terminal->write("Rebooting in 2s");
	//print("Rebooting in ");
	//for(int i = 3; i > 0; i--) {
	//	printFmt("%d...", i);
	delay(2000);
	//}
	terminal->write("0!\n\r");
	
	esp_restart();
}