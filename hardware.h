/* 
 * File:   hardware.h
 * Author: Steffis PC
 *
 * Created on 21. April 2013, 01:34
 */

#ifndef HARDWARE_H
#define	HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define Fosc    64000000        // if using external resonator, we need this
    
/* Step 1:
 * Comment/UnComment modules as needed by application
 */

    // Core Modules. These should not be disabled
#define MOD_rtc                     // real time clock. needed for system timing
#define MOD_ADC                     // ad converters. needed to track physical environment
#define MOD_FlashFS                 // needed for access to internal eeprom
#define MOD_Console                 // TODO: abstraction layer for UART,Display and Input Modules
//#define MOD_UART                    // TODO: serial console
    
    // Additional Modules
//#define MOD_I2C                   // I2C Driver, not using MSSP module
//#define MOD_FlashFS_extSPI        // TODO: Driver for external SPI Flash
//#define MOD_FlashFS_extI2C        // Driver for I2C eeprom devices. unfinished
//#define MOD_SoftPWM               // TODO: PWM Output on any Pin, controlled by software timers.
//#define MOD_HardPWM               // TODO: PWM Output using the PIC hardware CCPWM module

    // Input Modules
//#define MOD_Input_KB_PS2          // unfinished but somehow working
//#define MOD_Input_keypad          // untested for long time, maybe incompatible with newer interface
//#define MOD_Input_mousewheel      // unfinished

    // Output Modules
#define MOD_Display                 // Abstraction Layer to unify access to display hardware
//#define MOD_Display_VFLD          // For PT6311 (or compatible) Serial Display Controller
#define MOD_Display_LCD_Uni         // For KS0073/HD44780 or compatible LCD, with arbitrary Pinout
//#define MOD_Display_LCD           // For KS0073 (or compatible) 4/8bit Display Controller, all Pins on one Port. more efficient, but restricted pinout
                                    // TODO: Make LCD_Uni compatible with LCD

    
/* Step 2:
 * define the Port-Pins you wish to use.
 */

// <editor-fold defaultstate="collapsed" desc="Modules that need no Portpins">
/*      These Modules do not need any further configuration, they are ready to use
 */
#ifdef MOD_rtc
    #define T1_preload_h    0b00010111
    #define T1_preload_l    90
    // preloads calculation:
    // ( 65536 - (2 register write) ) - ( (16M Fcyc) / 256 Prescale )= 3034
    // the 46k20 only has 1:8 prescaler for TMR1, so we need 1:32 software postscaler
    // TODO: check and recalculate these, they are taken from another project
    #include "./rtc.h"
#endif

#ifdef MOD_FlashFS
    // TODO: if no internal eeprom is present, emulate it in program flash
    #include "./FlashFS.h"
    #include "./eeprom.h"
#endif/* MOD_FlashFS */

#ifdef MOD_Console
    #define CON_width       22
    #define CON_lines       8
    #define CON_command     CON_lines-1     // use the last line of the buffer as command line
    #include "console.h"
#endif /* MOD_Console */
// </editor-fold>
    
// <editor-fold defaultstate="collapsed" desc="MOD_UART">
#ifdef MOD_UART
    #define RX_BUFF_SIZE    32              // also determines max command line
    #define TX_BUFF_SIZE    32              // save memory
    #include "./uart.h"
    #define BAUD_H          BAUD_H__300
    #define BAUD_L          BAUD_L__300
    #define TX_t            TRISCbits.TRISC6
    #define RX_t            TRISCbits.TRISC7
    #define UART_echo
#endif /* MOD_UART */
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="MOD_SoftPWM">
#ifdef MOD_SoftPWM
    // The SoftPWM Module will use the HF-Counters 0 and 1, 
    // for period and active time, respectively.
    // any digital output pin can be used for this module
    #define PWM_Pin     PORTCbits.RC2
    #define PWM_Tris    TRISCbits.TRISC2
#endif
// </editor-fold>
    
// <editor-fold defaultstate="collapsed" desc="Analog Keypad Module">    
#ifdef MOD_Input_keypad       // TODO: also declare the AD-channel to use here. maybe use 0 by default.
    //  Keypad
    #include "./keypad.h"
    #define Scan1                       PORTDbits.RD7
    #define Scan1_tr                    TRISDbits.TRISD7
    #define Scan2                       PORTDbits.RD6
    #define Scan2_tr                    TRISDbits.TRISD6
    #define Scan3                       PORTDbits.RD5
    #define Scan3_tr                    TRISDbits.TRISD5
#endif /* MOD_Input_keypad */
// </editor-fold>
  
// <editor-fold defaultstate="collapsed" desc="Mousewheel Input Module">
#ifdef MOD_Input_mousewheel
    //  Wheel
    #define Wheel1                      PORTAbits.RA6
    #define WH1_tr                      TRISAbits.TRISA6
    #define Wheel2                      PORTEbits.RE2
    #define WH2_tr                      TRISEbits.TRISE2
#endif /* MOD_Input_mousewheel */
//</editor-fold>
    
// <editor-fold defaultstate="collapsed" desc="PS2 Keyboard Module">
#ifdef MOD_Input_KB_PS2
    #include "ps2-keyboard.h"

    #define KB_PS2_Keynum               104
    #define PS2_Clock                   PORTBbits.RB0                                        // This must be an IoC-Pin !!
    #define PS2_Data                    PORTCbits.RC0
    #define PS2_Clock_tr                TRISBbits.TRISB0
    #define PS2_Data_tr                 TRISCbits.TRISC0
#endif /* MOD_Input_KB_PS2 */
//</editor-fold>
    
// <editor-fold defaultstate="collapsed" desc="ADC Configuration">
#ifdef MOD_ADC
    // ADC Configuration
#endif /* MOD_ADC */
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Display Modules">
#ifdef MOD_Display
#include "./display.h"
    
// <editor-fold defaultstate="collapsed" desc="PT6311 compatible VFLD Display">
#if defined MOD_Display_VFLD
    // VFL Display with PT6311 or compatible Controller
    #include "./VFLD.h"
    
    #define VFLD_STB                    PORTDbits.RD3           // RA4  -> Strobe
    #define VFLD_STB_TRIS             TRISDbits.TRISD3
    #define VFLD_CLK                    PORTDbits.RD2           // RB3  -> Clock
    #define VFLD_CLK_TRIS             TRISDbits.TRISD2
    #define VFLD_DATA                   PORTDbits.RD4           // RD0  -> Data
    #define VFLD_DIN_TRIS             TRISDbits.TRISD4
// </editor-fold>
    
// <editor-fold defaultstate="collapsed" desc="HD44780/KS0073 compatible LCD, 1-Port">    
#elif defined MOD_Display_LCD
    // LCD with KS0073 or compatible Controller
    // support 4bit-mode using one full port, 4 bit data on upper Pins, clock, enable, rw, +1 for switching the led backlight
    // this will be hardcoded to 4x20 Display for now
    #include "./lcd.h"
    
    #define LCD_Port                    PORTD
    #define LCD_Tris                    TRISD
    #define LCD_RW                      PORTDbits.RD2
    #define LCD_RS                      PORTDbits.RD1
    #define LCD_Enable                  PORTDbits.RD3
    #define LCD_Backlight               PORTDbits.RD0
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="HD44780/KS0073 compatible LCD, universal Pinout">    
#elif defined MOD_Display_LCD_Uni
    // HD44780 compatible LCD
    // 4-bit Mode, arbitrary Pinout
    // this will be only tested for 4x20 Display for now
    #define LCDuni_Lines    4
    #define LCDuni_Cols     20
    #include "./lcd_uni.h"
    
    #define TRIS_EN         TRISDbits.TRISD5
    #define TRIS_RW         TRISDbits.TRISD6
    #define TRIS_RS         TRISDbits.TRISD7
    #define TRIS_D7         TRISDbits.TRISD3
    #define TRIS_D6         TRISDbits.TRISD2
    #define TRIS_D5         TRISDbits.TRISD1
    #define TRIS_D4         TRISDbits.TRISD0
    #define TRIS_BL         TRISDbits.TRISD4

    #define	LCD_EN          PORTDbits.RD5       // Enable
    #define	LCD_RW          PORTDbits.RD6       // R/W
    #define	LCD_RS          PORTDbits.RD7       // Register Select
    #define	LCD_D7          PORTDbits.RD3
    #define	LCD_D6          PORTDbits.RD2
    #define	LCD_D5          PORTDbits.RD1
    #define	LCD_D4          PORTDbits.RD0
    #define LCD_BL          PORTDbits.RD4       // Backlight Switch    
#endif /* MOD_Display_<*> */
// </editor-fold>

#endif /* MOD_Display */
// </editor-fold>
    
// <editor-fold defaultstate="collapsed" desc="I2C BUS BitBang">
#ifdef MOD_I2C 
    #include "i2c_bitbang.h"
    #define I2C_Buffer                  64
    #define SCL_t                       TRISCbits.TRISC3
    #define SDA_t                       TRISCbits.TRISC4
    #define SCL                         PORTCbits.RC3
    #define SDA                         PORTCbits.RC4
#endif /* MOD_I2C */

#ifdef MOD_FlashFS_extI2C
    #include "i2c_bitbang.h"
    #include "eeprom_i2c.h"
#endif /* MOD_FlashFS_extI2C */
    // </editor-fold>
   
    // These configure EEPROM size for each processor
    // can also be used to configure other chip-specific stuff
    
// <editor-fold defaultstate="collapsed" desc="Processor specific #defines">    
#if defined(__18F46K20)
    // <editor-fold defaultstate="collapsed" desc="PIC18F 46K20">
    #define	EE_1k
// </editor-fold>
#elif defined(__18F4550)
    // <editor-fold defaultstate="collapsed" desc="PIC18F 4550">
#define EE_256
// </editor-fold>
#endif
// </editor-fold>
        
#ifdef	__cplusplus
}
#endif

#endif	/* HARDWARE_H */

