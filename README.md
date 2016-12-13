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
  
  
