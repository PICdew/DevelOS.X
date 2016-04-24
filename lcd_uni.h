/*
**	Project:	4-Bit LCD
**	Purpose:	Interface a Hitachi-compatible LCD screen to an 8-bit
**				microcontroller making use of 7 bits of any port.  Timer
**				delay functions are used as the ready flag polling
**				is not needed in some applications.
**
**	Author:		Steven Pickles
**	Date:		Friday, April 22, 2005
 *
 *  Mostly rewritten by Stefanie Rausch, July 2014
 *
 *  Changes:        - ported from avr to pic
 *                  - Work in single-bit mode, so you can
 *                    use any pins of any ports in any order
 *                  - implemented soft-reset sequence
 *                  - all commands configurable via #define
 *                  - translate()-function to adapt to lcd charset
 *                  - paranoid init-sequence with ready-check
 *                  - now check the busy flag
 * 
 * Integrated into DevelOS by Stefanie Rausch, April 2016, WIP
*/
#ifndef LCD_UNI_H
#define	LCD_UNI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "./DevelOS.h"

#ifdef MOD_Display_LCD_Uni
    extern struct LCD_Display {
        unsigned char Buffer[LCDuni_Lines][LCDuni_Cols];
        /* a buffer for a 4x20 lcd
         * do not confuse this one with 
         * struct Disp { unsigned char Buffer[BufferLines][20]; ... } Display;
         * in "display.h".
         * this one is the hardware buffer. If neccessary, this is the place to translate 
         * characters from ascii to whatever the display speaks. My current Display 
         * is a Electronic Assembly EAW204-NLED, which happens to speak ascii.
         */
        unsigned char LineOffset[LCDuni_Lines];                                            // for each line, save the memory-offset
        struct {
            unsigned char width;
            unsigned char height;
        } Dimensions;
        char Busy;                                                              // set this while writing to the lcd, to block any writes to buffer
        char Light;                                                             // for led-backlight
    } LCD;              // a buffer for a 4x20 LCD
    
    char InitLCD(void);    
    void RefreshLCD(void);
    char LCD_Busy(void);
    void LCD_WriteCMD(const unsigned char byte);                                // send command to Display
    void LCD_WriteCGByte(const unsigned char byte, const unsigned char adress);  // Write a byte to CG-RAM
    void LCD_WriteByte(const unsigned char byte, const unsigned char adress);    // Write a byte to specified adress in Display RAM
    unsigned char LCD_ReadByte(const unsigned char adress);                      // Read a byte from specified adress in Display RAM
    void LCD_clr(void);
    void writeNibbleToLCD(char nibble);
    char readNibbleFromLCD();

extern const rom unsigned char LCD_Command[8];                                    // Store all the command-codes in here
    #define LCD_CMD_clr     0   // CMD 0    :   Clear all Display, reset cursor
    #define LCD_CMD_ret     1   // CMD 1    :   Return cursor and viewport to 0
    #define LCD_CMD_ems     2   // CMD 2    :   Entry Mode Set (OR in Setting[0])
    #define LCD_CMD_dc      3   // CMD 3    :   Display On/Off/Control (OR in Setting[1])
    #define LCD_CMD_cds     4   // CMD 4    :   Cursor/Display Shift
    #define LCD_CMD_fs      5   // CMD 5    :   Function Set (OR in Setting[2])
    #define LCD_CMD_sca     6   // CMD 6    :   Set CG-RAM Address. OR in the address
    #define LCD_CMD_sda     7   // CMD 7    :   Set DD-RAM Address. OR in the address
    // defines for the LCD Timings
    #define LCD_PON_1       40000000    // Power on Delay (40 ms)
    #define LCD_PON_2       4100000     // Reset Delay (4.1 ms))
    #define LCD_PON_3       100000      // Init Delay (100 µs)
    #define LCD_E_CYC       400         // Min LCD Enable Cycle Time /2 in ns
    #define LCD_T_AS        100         // Adress setup time
    #define LCD_T_DSW       100         // Data setup Time
    #define LCD_T_AH        10          // Adress Hold Time

extern const rom unsigned char LCD_Setting[3];
    
#endif

#ifdef	__cplusplus
}
#endif

#endif /* LCD_UNI_H */