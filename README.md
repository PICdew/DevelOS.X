# DevelOS

Started 27.04.2013 as an OS for Microchip PIC MCUs.

It is designed to be tiny but powerfull. 
Almost everything is modular, so it can be fitted to any project.
Modularization is done mostly with #define statements in some header files.

hardware.h      : Pin assignments and module activation
DevelOS.h       : internal parameters and data structures for the OS
configbits.h    : The config #pragmas for the particular PIC 
events.h        : these are the hooks to the process pipeline
display.h       : abstraction layer between OS and Display Hardware.
FlashFS.h       : abstraction layer between OS and Storage 
keypad.h        : driver for matrix keypad. analog high speed readout. 
ps2-keyboard.h  : i had this one semi-working, but without parity check and re-send.
lcd_uni.h       : HD44780/KS0073 or compatible 4bit LCD Driver. with arbitrary Pinout
lcd.h           : HD44780/KS0073 or compatible 4bit LCD Driver. all Pins on one Port
VFLD.h          : 

There are drivers for some common hardware, like HD44780/KS0073 LCD-Displays.
Other modules exist for internal peripherals, like EEPROM, ADC and so on.
Some modules are purely software, like the RTC.

I am using a lot of editor folds to keep the code readble.
There is a long TODO List, but it is already working. 
currently, i am coding the i2c-bus functions
