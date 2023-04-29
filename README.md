# Agon MOS ZDI flash utility
## Overview
This utility provides a means to (baremetal) program a new MOS firmware to a AgonLight ez80 flashchip. This can be performed in case of an initially erased or corrupted flash, or having previously flashed an incompatible MOS version.
The premise of this utility is that there is no running firmware in the ez80 itself, MOS or otherwise. The only means to program the ez80 flash in this case is to use the Zilog Debug Interface (ZDI), using a 6-pin connector on the AgonLight board.

The ZDI can be used with one of the Zilog Smart Cable options, together with the Zilog Developer Studio II software to program a new MOS firmware, or use this utility without needing such a cable.

The utility needs to be flashed to the onboard ESP32 IC, normally used as graphics processor (VDP). When the ESP32 IC is done running the utility and flashing the MOS firmware to the ez80 flash, it needs to be flashed again with the VDP code, so resume it's role as graphics processor.

To enable the utility access to the ZDI interface, it needs to cables connected as detailed below. After installation, the user simply places the required MOS firmware on the SD card, which the utility picks up and flashes.

## Flashing the utility
The utility can be flashed in multiple ways:
1. By using a the graphical 'ESP32 Flash Download Tool' from Espressif. This is probably the easiest option for most people, but requires a Windows PC. Follow the description in the next paragraph
2. By using one of the provided batch/shell scripts provided in the [batchfile-flash](https://github.com/envenomator/agon-vdpflash/tree/master/batchfile-flash) folder. You'll probably need to tweak the serial setting for your specific COM/UART port. The rest of the settings are included in the script, which are fed to the Espressif 'esptool', also provided in the folder. For repeated, single-command flashing of the ESP32, this is my personal preference, but can be a little more challenging for users.
3. By installing PlatformIO and building/flashing the utility to the ESP32. Describing this process is outside the scope of this document. There are some [excellent tutorials](https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/) to be found online to describe the setup of PlatformIO. After setting up PlatformIO, open up the git folder and build the utility from source.

## Flashing using the ESP Flash download tool
The latest version of this tool can be downloaded [from the Espressif support website](https://www.espressif.com/en/support/download/other-tools?keys=&field_type_tid%5B%5D=13)

After installation of the Espressif ESP Flash download tool, run it and fill in the fields exactly as follows, pointing the binaries where you downloaded this git to on disk. Binaries to flash are in the 'batchfile-flash' folder, which I put in 'C:\bin' in this example:

![espressif settings](https://github.com/envenomator/agon-vdpflash/tree/master/media/esp32flash.PNG)

## Requirements
1. Two (dupont female-female) cables need to be connected between the external GPIO and ZDI ports of the AgonLight board. Please see detailed description below
2. The required MOS version to flash needs to be placed on the SD card's root filesystem, with filename "MOS.bin"
3. The board needs to be reset after inserting the SD card.

## Cabling details
### ZDI connector
![zdi connector](https://github.com/envenomator/agon-vdpflash/tree/master/media/zdi.png)

### Original AgonLight GPIO header
![agonlight](https://github.com/envenomator/agon-vdpflash/tree/master/media/originalagonlight.png)

### Olimex AgonLight2 GPIO IDC box header
![agonlight2](https://github.com/envenomator/agon-vdpflash/tree/master/media/olimexagonlight2.png)

