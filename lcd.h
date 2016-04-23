/*      DevelOS
 *
 * File:    lcd.h
 * Module:  MOD_Display_LCD
 * Author:  Steffis PC
 * Destiny: Generic HD44780/KS00073-style LCD Driver
 *
 * Created on 27. April 2013, 20:26
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "./DevelOS.h"

#ifdef MOD_Display_LCD

    extern struct LCD_Display {
        unsigned char Buffer[4][20];                                            // a buffer for a 4x20 lcd
        unsigned char LineOffset[4];                                            // for each line, save the memory-offset
        struct {
            unsigned char width;
            unsigned char height;
        } Dimensions;
        char Busy;                                                              // set this while writing to the lcd, to block any writes to buffer
        char Light;                                                             // for led-backlight
    } LCD;

    void InitLCD(void);
    void RefreshLCD(void);
    char LCD_Busy(void);
    void LCD_WriteCMD(const unsigned char byte);                                // send command to Display
    void LCD_WriteCGByte(const unsigned char byte, const unsigned int adress);  // Write a byte to CG-RAM
    void LCD_WriteByte(const unsigned char byte, const unsigned int adress);    // Write a byte to specified adress in Display RAM
    unsigned char LCD_ReadByte(const unsigned int adress);                      // Read a byte from specified adress in Display RAM
    void LCD_clr(void);
//    void LCD_print(const unsigned char* string[]);
//    void LCD_value(unsigned int value);

    extern const rom unsigned char LCD_Command[8];                                    // Store all the command-codes in here
    #define LCD_CMD_clr     0   // CMD 0    :   Clear all Display, reset cursor
    #define LCD_CMD_ret     1   // CMD 1    :   Return cursor and viewport to 0
    #define LCD_CMD_ems     2   // CMD 2    :   Entry Mode Set (OR in Setting[0])
    #define LCD_CMD_dc      3   // CMD 3    :   Display On/Off/Control (OR in Setting[1])
    #define LCD_CMD_cds     4   // CMD 4    :   Cursor/Display Shift
    #define LCD_CMD_fs      5   // CMD 5    :   Function Set (OR in Setting[2])
    #define LCD_CMD_sca     6   // CMD 6    :   Set CG-RAM Address. OR in the address
    #define LCD_CMD_sda     7   // CMD 7    :   Set DD-RAM Address. OR in the address
    #define LCD_BSY         0x80            //  Busy Pattern
    extern const rom unsigned char LCD_Setting[3];
#endif /* MOD_Display_LCD */

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

