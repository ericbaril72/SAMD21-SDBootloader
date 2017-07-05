# SAMD21-SDBootloader
Code Publishing

Taken from Atmel SAMD20 SD bootloader code.

The original SD bootloader runs on the SAMD EXPLAINED PRO eval platform.
This eval board uses the SAMD20J18 package.

Lots of PIN remapping to match the Adafruit SAMD21- WINC1500 Board.
Bootloader size is roughly 28-29KB
   --> do not forget to update the BOOT_PROT fuses to 0x07 to remove bootloader protection !
   
   APP_START addr is 0x8000
   update your boards.txt ( ARDUINO ) file so that the linker will start at addr 0x8000

On startup, if the App_addr is empty, the bootloading process is mandatory.
It will check for SD card ( CS_pin10 ) of the Adafruit platform.

IF A0 is HIGH, the file used for bootloading will be MDL.bin
IF A0 is LOW, the file will be fd.bin ( factory default )

once the file has been written to memory ( 0x8000  and ++ ), it activates the WDT and waits for a reset.
on startup, if the reset_CAUSE is WDT, it jumps to the APP_START addrs


NOTICE:
  The presence of a J-LINK programmer on the SAMD21 device seems to interact with the bootlader ability to "Start" the application after the WDT_reset 
  Just beware !... remove the J-LINK if bootloading does not seems to start the application after flashing
  
 
 My latest toughts on this bottloader:
 It would be WAY better if it implemented a USB MSC ( mass storage class ) as well.
 I use this bootloader on the FEATHER_M0_WINC1500   +   SD_CARD_featherWing.
 
 After programming the CPU with the bootloader, I need to make sure the SD card contains the fd.bin or MDL.bin files.
 When I want to update the software, the CPU downloads a new firmware from the WEB ( via the WINC1500 wifi module ).
 Once in a while, I make changes to the software that screws up the file download process and the latest .BIN running is no longer able to download an updated BIN.  ( that sucks )
 
 So I have to open the casing, remove the SD card, write my new BINary, put it back i, close casing and reboot.
 ( Now you understand WHY I added the ability of selecting 2 different binaries --> fd.bin stands for factory default !!! )
 
 Having a USB MSC would expose the SD Card on the USB-cable as it if was a memory stick.  No longer would I need to open the casing .
 
 I am currently investigating the UF2 bootloader.  it Exposes USB as  Mass-Storage,  PC does the programming via standard drag-and-drop
 found here: https://github.com/Microsoft/uf2-samd21
 if you just want to try it out before getting into building it using makefile on a Unix machine ... ;-)
 pre-compiled UF2 bootloader.bin + blink.uf2 example :
 https://github.com/ericbaril72/SAMD21-SDBootloader/issues/1#issuecomment-312895350
 
 
 Stay tuned .... or ask questions
 
 
