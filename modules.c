#include "DevelOS.h"

/********************************************************\
 *              Runtime Module Detection                *
 *                                                      *
 * This function will allow the OS to detect active     *
 * modules at runtime. this will make it possible to    *
 * assign timers dynamically. it is basically a bridge  *
 * from #defines to runtime variables                   *
\********************************************************/

unsigned long detectModules(void)
{
    unsigned long temp;
    temp=0;
    
#ifdef MOD_rtc                     // real time clock. needed for system timing
    temp += 0b00000000000000000000000000000001;
#endif
#ifdef MOD_ADC                     // ad converters. needed to track physical environment
    temp += 0b00000000000000000000000000000010;
#endif
#ifdef MOD_FlashFS                 // needed for access to internal eeprom
    temp += 0b00000000000000000000000000000100;
#endif
#ifdef MOD_Console                 // TODO: abstraction layer for UART,Display and Input Modules
    temp += 0b00000000000000000000000000001000;
#endif
#ifdef MOD_UART                    // TODO: serial console
    temp += 0b00000000000000000000000000010000;
#endif
#ifdef MOD_I2C                   // I2C Driver, not using MSSP module
    temp += 0b00000000000000000000000000100000;
#endif
#ifdef MOD_FlashFS_extSPI        // TODO: Driver for external SPI Flash
    temp += 0b00000000000000000000000001000000;
#endif
#ifdef MOD_FlashFS_extI2C        // Driver for I2C eeprom devices. unfinished
    temp += 0b00000000000000000000000010000000;
#endif
#ifdef MOD_SoftPWM               // TODO: PWM Output on any Pin, controlled by software timers.
    temp += 0b00000000000000000000000100000000;
#endif
#ifdef MOD_HardPWM               // TODO: PWM Output using the PIC hardware CCPWM module
    temp += 0b00000000000000000000001000000000;
#endif
#ifdef MOD_Input_KB_PS2          // unfinished but somehow working
    temp += 0b00000000000000000000010000000000;
#endif
#ifdef MOD_Input_keypad          // untested for long time, maybe incompatible with newer interface
    temp += 0b00000000000000000000100000000000;
#endif
#ifdef MOD_Input_mousewheel      // unfinished
    temp += 0b00000000000000000001000000000000;
#endif
#ifdef MOD_Display                 // Abstraction Layer to unify access to display hardware
    temp += 0b00000000000000000010000000000000;
#endif
#ifdef MOD_Display_VFLD          // For PT6311 (or compatible) Serial Display Controller
    temp += 0b00000000000000000100000000000000;
#endif
#ifdef MOD_Display_LCD_Uni         // For KS0073/HD44780 or compatible LCD, with arbitrary Pinout
    temp += 0b00000000000000001000000000000000;
#endif
#ifdef MOD_Display_LCD           // For KS0073 (or compatible) 4/8bit Display Controller, all Pins on one Port. more efficient, but restricted pinout
    temp += 0b00000000000000010000000000000000;
#endif                                    // TODO: Make LCD_Uni compatible with LCD
    
    return temp;
}
