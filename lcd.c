/*      DevelOS
 *
 * File:    lcd.c
 * Module:  MOD_Display_LCD
 * Author:  Steffis PC
 * Destiny: Generic HD/KS73-style LCD Driver
 *
 * Created on 27. April 2013, 20:26
 */
#include "./DevelOS.h"

#ifdef MOD_Display_LCD

void InitLCD(void)
{
    char i;
    LCD_Tris=0x00;
    LCD_Port=0x00;

    OS_delay_ns(15000);                                                         // 15 ms Power-On Delay for LCD
    LCD_WriteCMD(0x30);
    OS_delay_ns(15000);                                                         // 15 ms Power-On Delay for LCD
    LCD_WriteCMD(0x30);
    OS_delay_ns(15000);                                                         // 15 ms Power-On Delay for LCD
    LCD_WriteCMD(0x32);
    OS_delay_ns(15000);                                                         // 15 ms Power-On Delay for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_fs] | LCD_Setting[2]);                     // Command: Function set
    OS_delay_ns(5000);                                                          // 5 ms for LCD to switch to 4-bit
    LCD_WriteCMD(LCD_Command[LCD_CMD_fs] | LCD_Setting[2]);                     // Command: Function set (again, just to be sure the lower nibble arrived)
                                                                                // Now, we can use the Busy-Flag. But we do the Variable Stuff first,                                                                                // while the LCD is still thinking
    for(i=0;i<20;i++)
    {
        LCD.Buffer[0][i]=0;
        LCD.Buffer[1][i]=0;
        LCD.Buffer[2][i]=0;
        LCD.Buffer[3][i]=0;
    }

    LCD.Busy=0;
    LCD.Dimensions.height=4;
    LCD.Dimensions.width=20;
    LCD.Light=1;
    LCD.LineOffset[0]=0x00;
    LCD.LineOffset[1]=0x40;
    LCD.LineOffset[2]=0x14;
    LCD.LineOffset[3]=0x54;

    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_dc] | LCD_Setting[1]);                     // Command: Display Control
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_clr]);                                     // Command: Clear Display
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_ems] | LCD_Setting[0]);                    // Command: Entry Mode Set
                                                                                // LCD should be in Ready State Now
                                                                                // check this by writing and re-reading
                                                                                // a byte to/from DD-Ram. Re-Init if no match
                                                                                // (i call this brute-force-init. just keep trying until success)
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteByte('!',0);                                                       // write a byte
    while(LCD_Busy());                                                          // wait for LCD
    if(LCD_ReadByte(0) != '!')
    {
        InitLCD();
    }
    else
    {
        RefreshLCD();
    }
}

void RefreshLCD(void)
{
    char x,y;

    while(LCD_Busy());
    //LCD_WriteCMD(LCD_Command[LCD_CMD_clr]);

    for(y=0;y<LCD.Dimensions.height;y++)
    {
        for(x=0;x<LCD.Dimensions.width;x++)
        {
            while(LCD_Busy());
            LCD_WriteByte(LCD.Buffer[y][x], (LCD.LineOffset[y]+x) );
        }
    }
}

void LCD_WriteCMD(const unsigned char byte)                                     // send a command to LCD Controller
{
        LCD_Tris &= 0x0f;               // set upper nibble output
        LCD_Port &= 0x0f;               // set upper nibble zero
        LCD_Port |= byte&0xf0;          // OR in the data
        LCD_RS = 0;                     // Set control bits
        LCD_RW = 0;                     // RS=0 & RW=0 --> Command
        OS_delay_ns(500);
        LCD_Enable = 1;                 // Clock nibble into LCD
        OS_delay_ns(500);
        LCD_Enable = 0;
        LCD_Port &= 0x0f;               // set upper nibble zero
        LCD_Port |= ((byte<<4)&0xf0);   // OR in the other nibble
        OS_delay_ns(500);
        LCD_Enable = 1;                 // Clock nibble into LCD
        OS_delay_ns(500);
        LCD_Enable = 0;
        LCD_Tris |= 0xf0;               // set upper nibble input
        // Done
}

char LCD_Busy(void)                                                             // check if LCD is Busy
{
    LCD_RW = 1;                         // Set the control bits for read
    LCD_RS = 0;
    OS_delay_ns(500);
    LCD_Enable = 1;                      // Clock in the command
    OS_delay_ns(500);
    if(LCD_Port&LCD_BSY)
    {
        LCD_Enable = 0;                 // Reset clock line
        OS_delay_ns(500);
        LCD_Enable = 1;                 // Clock out other nibble
        OS_delay_ns(500);
        LCD_Enable = 0;
        LCD_RW = 0;                     // Reset control line
        return 1;                       // Return TRUE
    }
    else                                // Busy bit is low
    {
        LCD_Enable = 0;                 // Reset clock line
        OS_delay_ns(500);
        LCD_Enable = 1;                 // Clock out other nibble
        OS_delay_ns(500);
        LCD_Enable = 0;
        LCD_RW = 0;                     // Reset control line
        return 0;                       // Return FALSE
    }
}

void LCD_WriteByte(const unsigned char byte, const unsigned int adress)         // Write a byte to specified address in Display RAM
{
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_sda] | adress);                            // Set DD-RAM adress
    while(LCD_Busy());                                                          // wait for LCD
    LCD_Tris &= 0x0F;                                                           // set upper nibble output
    LCD_Port &= 0x0F;                                                           // set upper nibble zero
    LCD_Port |= byte & 0xF0;                                                    // OR in the data
    LCD_RS = 1;                                                                 // Set control bits:
    LCD_RW = 0;                                                                 // RS=1 & RW=0 --> Write to Ram
    OS_delay_ns(500);
    LCD_Enable = 1;                                                             // Clock nibble into LCD
    OS_delay_ns(500);
    LCD_Enable = 0;
    LCD_Port &= 0x0F;                                                           // set upper nibble zero
    LCD_Port |= ( (byte<<4) & 0xF0);                                            // OR in the other nibble
    OS_delay_ns(500);
    LCD_Enable = 1;                                                             // Clock nibble into LCD
    OS_delay_ns(500);
    LCD_Enable = 0;
    LCD_Tris |= 0xF0;                                                           // set upper nibble input
        // Done
}

void LCD_WriteCGByte(const unsigned char byte, const unsigned int adress)       // Write a byte to CG-RAM address
{
    LCD_WriteCMD(LCD_Command[LCD_CMD_sca] | adress);                            // Set CG-RAM adress
    while(LCD_Busy());                                                          // wait for LCD
    LCD_Tris &= 0x0F;                                                           // set upper nibble output
    LCD_Port &= 0x0F;                                                           // set upper nibble zero
    LCD_Port |= byte & 0xF0;                                                    // OR in the data
    LCD_RS = 1;                                                                 // Set control bits:
    LCD_RW = 0;                                                                 // RS=1 & RW=0 --> Write to Ram
    OS_delay_ns(500);
    LCD_Enable = 1;                                                             // Clock nibble into LCD
    OS_delay_ns(500);
    LCD_Enable = 0;
    LCD_Port &= 0x0F;                                                           // set upper nibble zero
    LCD_Port |= ( (byte<<4) & 0xF0);                                            // OR in the other nibble
    OS_delay_ns(500);
    LCD_Enable = 1;                                                             // Clock nibble into LCD
    OS_delay_ns(500);
    LCD_Enable = 0;
    LCD_Tris |= 0xF0;                                                           // set upper nibble input
        // Done
}

unsigned char LCD_ReadByte(const unsigned int adress)                           // Read a byte from specified address in Display RAM
{
    unsigned char byte=0;

    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_sda] | adress);                            // Set DD-RAM adress
    LCD_RS = 1;                                                                 // Set control bits:
    LCD_RW = 1;                                                                 // RS=1 & RW=1 --> Read Data
    OS_delay_ns(500);
    LCD_Enable = 1;                                                             // Clock nibble out of LCD
    OS_delay_ns(500);
    byte = LCD_Port & 0xF0;                                                // read upper nibble
    LCD_Enable = 0;                                                             // Reset the clock line
    OS_delay_ns(500);
    LCD_Enable = 1;                                                             // Clock nibble out of LCD
    OS_delay_ns(500);
    byte |= (LCD_Port>>4) & 0x0F;                                                    // Read lower nibble
    // Clean up
    LCD_RS = 0;                                                                 // Reset control bits
    LCD_RW = 0;
    LCD_Enable = 0;
    return(byte);                                                               // Return the data byte
}

void LCD_clr(void)
{
    char i;

    for(i=0;i<20;i++)
    {
        LCD.Buffer[0][i]=' ';
        LCD.Buffer[1][i]=' ';
        LCD.Buffer[2][i]=' ';
        LCD.Buffer[3][i]=' ';
    }
    // additionally, send CLR-command to display
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_clr]);                                     // Command: Clear Display
}

const rom unsigned char LCD_Command[8] = {
    0b00000001,         // CMD 0    :   Clear all Display, reset cursor
    0b00000010,         // CMD 1    :   Return cursor and viewport to 0, leave DDram alone
    0b00000100,         // CMD 2    :   Entry Mode Set
                        // CMD 2,b0 :   Shift Display on write
                        // CMD 2,b1 :   Move Cursor on write
    0b00001000,         // CMD 3    :   Display On/Off/Control
                        // CMD 3,b0 :   Cursor Blink
                        // CMD 3,b1 :   Cursor On/Off
                        // CMD 3,b2 :   Display On/Off
    0b00010000,         // CMD 4    :   Cursor/Display Shift
                        // CMD 4,b2 :   Right/Left
                        // CMD 4,b3 :   Shift/Cursor
    0b00010000,         // CMD 5    :   Function Set
                        // CMD 5,b2 :   Character font
                        // CMD 5,b3 :   Number of Lines
                        // CMD 5,b4 :   4/8bit Mode
    0b01000000,         // CMD 6    :   Set CG-RAM Address. OR in the address
    0b10000000          // CMD 7    :   Set DD-RAM Address. OR in the address
};

const rom unsigned char LCD_Setting[3] = {
    // Set the right values here, they will be or'ed into the right words, the right time ;)
    0b00000000,         // CMD 2    :   Entry Mode Set
                        // CMD 2,b0 :   Shift Display on write
                        // CMD 2,b1 :   Move Cursor on write
    0b00000100,         // CMD 3    :   Display On/Off/Control
                        // CMD 3,b0 :   Cursor Blink
                        // CMD 3,b1 :   Cursor On/Off
                        // CMD 3,b2 :   Display On/Off
    0b00001000,         // CMD 5    :   Function Set
                        // CMD 5,b2 :   Character font
                        // CMD 5,b3 :   Number of Lines
                        // CMD 5,b4 :   4/8bit Mode
};

#endif /* MOD_Display_LCD */

