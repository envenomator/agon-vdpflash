@ECHO OFF
FOR /f "tokens=4" %%A IN ('mode^|findstr "COM[0-9]*:"') DO set port=%%A
FOR /f "tokens=1 delims=:" %%A IN ("%port%") DO set port=%%A
ECHO Flashing to ESP32...
".\esptool.exe" --chip esp32 --port "%port%" --baud 921600  --before default_reset --after hard_reset write_flash  -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 ".\video.ino.bootloader.bin" 0x8000 ".\video.ino.partitions.bin" 0x10000 ".\firmware.bin" 