Making binary firmware available
================================

If we need to release updates to the public and provide an easy way to flash the firmware to their badge we can do that by building and packaging up the files and then putting it up somewhere public for users to download and flash. The process isn't overly complicated but does require a few things.

## Building and packaging files

To get the files needed for flashing we just have to build them in PlatformIO. If you have PlatformIO installed in VSCode you can use commands or for point-and-click you can go to the extension pane on the left and build everything there. Make sure you build the filesystem image as well as building the project firmware itself. Once you've done that just navigate to the `.pio/build/esp32-s2-saola-1` directory that is generated by PlatformIO within the project directory (`firmware` in the case of
this repository). Then just create an archive containing the following files:
- `bootloader.bin` (the bootloader)
- `partitions.bin` (the partition table/layout)
- `firmware.bin` (the actual firmware code)
- `spiffs.bin` (the contents of the `data` directory packaged as a SPIFFS partition image)

Once those are packaged up just upload them somewhere for users to grab for flashing to their badge. At the time of writing this I am running these commands to create a debug version (`-DCORE_DEBUG_LEVEL=4` in `platformio.ini`) and a non-debug version (`-DCORE_DEBUG_LEVEL=2`):
```sh
# Make sure we're in the firmware subdirectory
cd firmware

# Build a debug version by setting `-DCORE_DEBUG_LEVEL=4` and then building the firmware. Then package it like this:
$ tar -czvf builds/firmware-debug.tar.gz -C .pio/build/esp32-s2-saola-1 bootloader.bin firmware.bin partitions.bin spiffs.bin

# Build a non-debug version by setting `-DCORE_DEBUG_LEVEL=2` and do the same:
$ tar -czvf builds/firmware.tar.gz -C .pio/build/esp32-s2-saola-1 bootloader.bin firmware.bin partitions.bin spiffs.bin
```

## Flashing the firmware

In order to flash the firmware you will need to first make sure you have all of those files extracted into a directory somewhere that you can work from. Once you have them somewhere you will need a tool to flash them with. The available options are mostly going to be either using the `esptool` python library or using a web serial flasher that lets you do it in a point-and-click fashion via the web browser.

### ESPTool

First you'll need to install `esptool`. The simplest way to do that for any given platform is to make sure you have Python installed and then install the package with:
```sh
python -m pip install esptool
```

If you are on macOS and have Homebrew you can do:
```sh
brew install esptool
```

#### Flashing

Once you have `esptool.py` available in your path you can flash with it by doing the following steps:
1. Plug your badge into your computer via the USB port (make sure you have a proper microUSB data cable)
2. See if the serial port shows up (e.g. `/dev/cu.usbmodem01` on macOS, `/dev/ttyUSB0` on Linux, `COM1` on Windows, etc.). If you do not see a serial port show up then you may need to put the chip into download mode by holding the `FLASH` button down while pressing the `RESET` button on the back.
3. Once you know the port and can see it you are ready to flash it. Make sure you are in a terminal with your current directory being the location of the firmware files you extracted and then run the following making sure to replace `/dev/cu.usbmodem01` with your serial port:
```sh
esptool.py --chip esp32s2 --port /dev/cu.usbmodem01 --baud 460800 --before default_reset --after hard_reset write_flash \
    0x1000   bootloader.bin \
    0x8000   partitions.bin \
    0x10000  firmware.bin   \
    0x290000 spiffs.bin
```

### Web flasher (GUI option)

For this you will need a browser that supports the WebSerial protocol... this mostly means something like Chrome or Opera. Then just pick a site like one of the following:
- https://esp.huhn.me/
- https://espressif.github.io/esptool-js/

For now I'll leave you to follow the directions for the site you choose... maybe later I'll add more detailed instructions here