# Rikta
STM32-Arduino-based 
Based on https://github.com/grafalex82/GPSLogger

# Hardware

Hardware used:
* 1.69" OLED 160x128 display UG-6028GDAB
* SD card slot

Connection schematics is TBD. Brief list of connections:
* Display is connected to pins:
	DSCL 	-> PA5
	DSDL 	-> PA7
	DSDO 	-> PA6
	D14  	-> PA0
	D13  	-> PA1
	D12  	-> PA2
	D11  	-> PA3
	D10  	-> PB0
	D9   	-> PB1
	RS  	-> PB10
	CSB  	-> PA4
        RESETB	-> PB11

# Software used

The device firmware is based on STM32-arduino (stm32duino) framework to access peripheral. At the same time FreeRTOS is used to drive simultaneous and concurrent tasks. 

Libraries used:
* Adafruit GFX and SEPS525-OLED libraries to drive the display

The project is using Arduino sketches concept. Atmel Studio + Visual Micro plugin is used to build all the stuff instead of ArduinoIDE. Due to limitations of arduino build process I will likely search for another build system option.

# Project Structure

* cmake - CMake toolchain files and project wide compiler settings
* Docs - various useful docs for project components. Just to keep them at a single location
* Libs - Used libraries. Used mostly a copy of original libraries, but have some minor tweaks and configuration changes
* Fonts - sources for fonts used in the project
* Src - application sources
* Test - unit test and other testing related stuff
* tools - various scripts


# How to build

CMake is used as a build system. Here is an example how to generate MinGW makefiles

```
mkdir <Builddir>
cd <Builddir>
cmake "-DTOOLCHAIN_PREFIX=<PathToArmGccCompiler>" -G "MinGW Makefiles" <PathToProject>
mingw32-make GPSLogger.bin
```

