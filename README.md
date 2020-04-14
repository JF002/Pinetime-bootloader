# pinetime-bootloader
This project is a sandbox to experiment with an MCUBoot-based bootloader for Pinetime.

The goal of this bootloader is to boot the application firmware and, if necessary, update the current image with a new one.
It does not integrate a BLE stack to actually download a new image to the flash memory. This will be the responsability of the application firmware.

The current implementation contains the bare-minimum to boot the NRF52832 MCU and run MCUBoot. 
The memory map is defined as followed:

 - SCRATCH AREA : 0x50000 (size : 0x3000)
 - PRIMARY SLOT : 0x60000 (size : 0x3000)
 - SECONDARY SLOT : 0x70000 (size : 0x3000)
 
This is completely arbitrary and does not reflect an actual use case (which will use the external SPI Flash as secondary).

For this to work, you'll have to flash an application into PRIMARY and SECONDARY slots (see https://github.com/JF002/Pinetime-bootloader-testapps).

**NOTE**: if you initialize Segger RTT in the bootloader, the target application won't be able to use it once it is started by the bootloader.
