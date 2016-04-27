# DevelOS

Started 27.04.2013 as an OS for Microchip PIC MCUs.

It is designed to be tiny but powerfull. 
Almost everything is modular, so it can be fitted to any project.

State right now: working on PIC 18F 46K20

Uses 805 Bytes RAM, 17953 Bytes ROM, 128 Bytes EEPROM 

Runs 5500 Loops per second @ 64 MHz in Standby Runlevel.

This includes RTC, 2 Hz Display Refresh Rate, 
8 Lines x 22 Console Buffer, 8 Events Pipeline Buffer, 
3 LF and 2 HF Software counters

Modularization is done mostly with #define statements in header files.
- hardware.h      : Pin assignments and module activation
- DevelOS.h       : internal parameters and data structures for the OS
- configbits.h    : The config #pragmas for the particular PIC 
- events.h        : these are the hooks to the process pipeline
- display.h       : abstraction layer between OS and Display Hardware
- FlashFS.h       : abstraction layer between OS and Storage 
- keypad.h        : driver for matrix keypad. analog high speed readout with low pincount
- ps2-keyboard.h  : i had this one semi-working, but without parity check and re-send
- lcd_uni.h       : Hitachi HD44780/Samsung KS0073 or compatible 4bit LCD Driver. with arbitrary Pinout
- lcd.h           : Hitachi HD44780/Samsung KS0073 or compatible 4bit LCD Driver. all Pins on one Port
- VFLD.h          : Princeton Technology PT6311 or compatible VFD Driver

There are drivers for some common hardware, like HD44780/KS0073 LCD-Displays.
These must be configured in hardware.h. Other modules exist for internal peripherals, like EEPROM, ADC and so on.
These may or may not have their own Parameters. Some modules are purely software, like the RTC. The RTC will propably get included into the core.

I am using a lot of editor folds to keep the code readble.

There is a long TODO List, but it is already working somehow. But i have not tested all the modules
with the actual software interfaces, so some modules might be broken. If you need one of them,
adapt the module to the interface, not the interface to the module.

Definitely working:
- Core 			: The Pipeline with event handling, interrupt handling and error handling
- RTC 				: Real time clock based on hardware timer. Runs smoothly and relatively precise
- Display	: artifacts came from mismatch of ROM/RAM based strings
- Console : Abstraction Layer between OS and Input/Output modules
- LCDuni  : Universal HD44780 Driver. totally independet of pinout. pins can be on diffrent ports
- HardPWM : Provide 2 PWM Channels using the Hardware PWM modules of the PIC
 
Definitely NOT working are:
- I2C 				: This is still unfinished and full of errors
- UART    : see above

Untested:
- VFLD    : this has been working long time ago, likely to be incompatible with newer interfaces
- LCD     : must be adapted to fit interface of LCDuni
- PS2     : has not been testet in ages, and was still unfinished back then
- Keypad  : needs adaption to new interfaces
