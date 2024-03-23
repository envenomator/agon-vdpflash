# Agon MOS/VDP ZDI flash utility
## Overview
This utility provides a means to (bare-metal) program a new MOS/VDP firmware to a AgonLight ez80 flashchip. This can be performed in case of an initially erased or corrupted flash, or having previously flashed an incompatible MOS version.
The premise of this utility is that there is no running firmware in the ez80 itself, MOS or otherwise. The only means to program the ez80 flash in this case is to use the Zilog Debug Interface (ZDI), using a 6-pin connector on the AgonLight board.

The ZDI can be used with one of the Zilog Smart Cable options, together with the Zilog Developer Studio II software to program a new MOS firmware, or through the use of this utility.

The utility needs to be flashed to the onboard ESP32 IC, normally used as graphics processor (VDP). When the ESP32 IC is done running the utility and flashing the MOS firmware to the ez80 flash, it (re)flashes itself with the VDP firmware file on the SD card, fully restoring functionality of the Agon system.

To enable the utility access to the ZDI interface, it needs two female/female dupont jumpercables connected as detailed below. After installation, the user simply places the required MOS and VDP firmware files on the SD card, which the utility picks up and flashes in the correct order.

## Preparation requirements
1. Download the contents of this repo to a folder and open a terminal window to the subfolder 'batchfile-flash'
2. Connect two (dupont female-female) cables between the external GPIO and ZDI ports of the AgonLight board. Please see detailed description below
3. Place the required **target** firmware files in the **root** directory of the microSD card:
	- MOS firmware - download the latest [Console8 MOS.bin](https://github.com/AgonConsole8/agon-mos/releases/latest/download/MOS.bin)
	- VDP firmware - download the latest [Console8 firmware.bin](https://github.com/AgonConsole8/agon-vdp/releases/latest/download/firmware.bin)
4. The Agon needs to be reset after inserting the SD card - just press the reset button.
5. Find out which serial/COM port shows up on your PC, after connecting your Agon using a USB cable and <em>note it down for later use</em>:
	- Under Windows, it shows up in the 'Device Manager', or you can type '[System.IO.Ports.SerialPort]::getportnames()' in a PowerShell terminal
	- Under Linux, you can find it by running 'dmesg | grep tty' in a shell
	- Under MacOS, you can find it by running 'ls /dev/tty.*' in Terminal
6. For the Olimex AgonLight2 - please make sure the jumpers on the board are placed like this: the **LEFT** jumper is unconnected/open/placed on just one pin, the **RIGHT** jumper is connected/closed/placed on both pins 

You can now proceed flashing the utility to the ESP32.

## Flashing the utility
The utility can be flashed in multiple ways:
1. RECOMMENDED: By using the supplied commandline script - this option is highly recommended, because there is very little chance of user-error. See below for either the Windows batch file, or Unix script
2. By using a the graphical 'ESP32 Flash Download Tool' from Espressif. This tool requires a Windows PC and is only recommended for experienced users. Follow the description in the next paragraph if you're sure what you are doing.
3. By installing PlatformIO and building/flashing the utility to the ESP32. Describing this process is outside the scope of this document. There are some [excellent tutorials](https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/) to be found online to describe the setup of PlatformIO. After setting up PlatformIO, open up the git folder and build the utility from source.

### Windows batch file
After changing directory to the 'batchfile-flash' folder, use the script with the following parameters:

    .\flash.bat [COM_PORT] <BAUDRATE>

### Unix script
After downloading the unix shell script, it's line-endings may have been mangled by git. This can be fixed by using

    sed -i -e 's/\r$//' flash.sh

It is probably also necessary to make the script itself executable by setting the execute bit to the file:

    chmod +x flash.sh

Assuming python3 has already been installed, please (as superuser) install the esptool.py utility using

    sudo pip install esptool


After changing directory to the 'batchfile-flash' folder, use the script (as superuser) with the following parameters:

    sudo ./flash.sh [SERIAL_PORT] <BAUDRATE>


## Flashing using the ESP Flash download tool
This option is only for experienced users, as the tool has a terrible UX design and settings are easily messed up.

The latest version of this tool can be downloaded [from the Espressif support website](https://www.espressif.com/en/support/download/other-tools?keys=&field_type_tid%5B%5D=13)

Connect your AgonLight board to your PC, using an appropriate USB interface cable. This will provide serial connectivity between the ESP32 on your AgonLight and the PC.

After installation of the Espressif ESP Flash download tool, run it and fill in the fields as follows:

Select the **ESP32** platform for *Chiptype*:

![espressif settings1](/media/flash-settings1.png)

Leave **WorkMode** at *Develop*:

![espressif settings2](/media/flash-settings2.png)

Next, on the main screen, take care to specify these exact addresses for:

|    Filename    | Address |
|:--------------:|--------:|
| bootloader.bin |  0x1000 |
| partitions.bin |  0x8000 |
|  firmware.bin  | 0x10000 |

these files can be found in the repo under the 'batchfile-flash' folder.

And **make sure** to **SELECT** the checkboxes on the left, to select all files to flash.

![espressif settings2](/media/flash-tool.png)

- SPI speed is 80Mhz
- SPI mode can be DIO, or QIO
- Please don't use the CombineBin/Default buttons
- Leave the *DoNotChgBin* option selected
- provide your own serial port details from the **DROPDOWN** and don't type it in.
 
Then press 'Start', wait for the tool to finish and follow the on-screen instructions on your VGA monitor connected to the AgonLight board.

## Cabling details
### ZDI connector
![zdi connector](/media/zdi.png)

### Original AgonLight GPIO header
![agonlight](/media/originalagonlight.png)

### Olimex AgonLight2 GPIO IDC box header
![agonlight2](/media/olimexagonlight2.png)

## Implementation details
This utility makes use of the excellent ZDI code from Mario Smit ([S0urceror](https://github.com/S0urceror/AgonElectronHAL)), many thanks for the hard work of getting this working with the onboard ESP32!

After the utility is flashed and boots up the ESP32, it opens the ZDI interface and tries to upload a 2nd stage utility (ez80 code) to the ez80 and places that in memory to execute. This 2nd stage opens the SD card interface, reads the "MOS.bin" file and programs it to ez80 flash. Finally the loader uploads the "VDP.bin" file through the serial back to this utility which then flashes itself.