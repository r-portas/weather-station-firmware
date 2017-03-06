# Weather Station Firmware

> The firmware for a weather station created in an engineering team project

## Hardware Requirements
- Should run on any Atmega 324

## Building

### Windows
- Download avr toolchain from [here](http://andybrown.me.uk/2015/03/08/avr-gcc-492/)
- Download the avrdude tool from [here](http://goo.gl/O7TVrN)
- Inside the avrdude folder, go to `driver-amd64/install-filter-win.exe` and install the driver for the AVR MK2 programmer
- Add the binaries to the system path (/bin and /avr/bin)
- Download and install Make for Windows
- Download Git Bash

### Mac
    - For Mac, download CrossPack [link](https://www.obdev.at/products/crosspack/download-de.html)
    - There is also a AVR package available via homebrew

- Run `make install` to build the program and write to the board
