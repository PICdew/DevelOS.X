/*
**	Project:	4-Bit LCD
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
*/
#include "DevelOS.h"

#ifdef MOD_Display_LCD_Uni

char position;

// <editor-fold defaultstate="collapsed" desc="char InitLCD(void)">
char InitLCD(void)
{
    char i,cmd,byte,x,y;

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

    // Set all Portpins to 0 except Backlight
    LCD_BL=1;
    LCD_D4=0;
    LCD_D5=0;
    LCD_D6=0;
    LCD_D7=0;
    LCD_RS=0;
    LCD_RW=0;
    LCD_EN=0;

    // Set all TRIS to Output
    TRIS_RS=0;
    TRIS_RW=0;
    TRIS_EN=0;
    TRIS_D7=0;
    TRIS_D6=0;
    TRIS_D5=0;
    TRIS_D4=0;
    TRIS_BL=0;
    
    /* Init Sequence from Hitachi HD44780U Datasheet       
     * 
     * If you need a diffrent sequence, put this one 
     * in a #ifdef block and wirte your own as #elif
     * That way, many Controllers can be supported by 
     * one source file
     */
    // Begin Init Sequence
    cmd = 0b00000011;           // Reset Command
    OS_delay_ns(LCD_PON_1);     // Power on Delay (40 ms)
    writeNibbleToLCD(cmd);
    OS_delay_ns(LCD_PON_2);     // Reset Delay (4.1 ms)
    writeNibbleToLCD(cmd);
    OS_delay_ns(LCD_PON_3);     // Init Delay (100 µs)
    writeNibbleToLCD(cmd);
    cmd =  0b00000010;          // Set Interface 4-bit
    writeNibbleToLCD(cmd);
    // Busy Flag should work now
    while(LCD_Busy());
    LCD_WriteCMD(LCD_Command[LCD_CMD_fs] | LCD_Setting[2]);                     // Command: Function set (again, just to be sure the lower nibble arrived)
    while(LCD_Busy());
    LCD_WriteCMD(LCD_Command[LCD_CMD_dc] | LCD_Setting[1]);                     // Command: Display Control
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_clr]);                                     // Command: Clear Display
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteCMD(LCD_Command[LCD_CMD_ems] | LCD_Setting[0]);                    // Command: Entry Mode Set
    /* LCD should be in Ready State Now
     * Check this by writing and re-reading a byte to/from DD-Ram. 
     * Re-Init if no match
     * I call this brute-force-init. Just keep trying until success
     */
    
    while(LCD_Busy());                                                          // wait for LCD
    LCD_WriteByte('!',0x0);                                                       // write a byte
    while(LCD_Busy());                                                          // wait for LCD
    OS_delay_ns(LCD_E_CYC*2);
    byte = LCD_ReadByte(0x0);
    if( byte != '!')
    {
        return 1;
    }
    else
    {
        // clear hardware Buffer
        for(y=0;y<LCDuni_Lines;y++)
        {
            for(x=0;x<LCDuni_Cols;x++)
            {
                LCD.Buffer[y][x]=' ';
            }
        }
        // set memory offsets
        LCD.LineOffset[0]=0x00;
        LCD.LineOffset[1]=0x40;
        LCD.LineOffset[2]=0x14;
        LCD.LineOffset[3]=0x54;
        LCD.Dimensions.height = LCDuni_Lines;
        LCD.Dimensions.width = LCDuni_Cols;
        LCD.Busy = 0;
        LCD.Light = 1;        
        return 0;
    }
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="void RefreshLCD(void)">
void RefreshLCD(void)
{
    unsigned char x,y,upperNibble,lowerNibble;
    
    if(LCD.Busy == 1)
    {
        return;
    }
    
    LCD.Busy = 1;
    for(y=0;y<LCD.Dimensions.height;y++)
    {
        unsigned char cmd = LCD_Command[LCD_CMD_sda] | LCD.LineOffset[y];
        while(LCD_Busy());
        LCD_WriteCMD(cmd);                  // Set DD-RAM adress
        for(x=0;x<LCD.Dimensions.width;x++)
        {
            while(LCD_Busy());
            LCD_RS = 1;                     // Set control bits:
            LCD_RW = 0;                     // RS=1 & RW=0 --> Write to Ram
            TRIS_D4=0;
            TRIS_D5=0;
            TRIS_D6=0;
            TRIS_D7=0;
            OS_delay_ns(LCD_T_AS);          
            upperNibble = LCD.Buffer[y][x] >> 4;
            lowerNibble = LCD.Buffer[y][x] & 0x0f;
            writeNibbleToLCD(upperNibble);
            OS_delay_ns(LCD_E_CYC);
            writeNibbleToLCD(lowerNibble);
            OS_delay_ns(LCD_T_AH);
            //LCD_WriteByte(LCD.Buffer[y][x], (LCD.LineOffset[y]+x) );
        }
    }
    LCD.Busy = 0;
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="char LCD_Busy(void)">
char LCD_Busy(void)                                                             // check if LCD is Busy
{
    unsigned char byte = 0x00;
	unsigned char upperNibble = 0x00;
	unsigned char lowerNibble = 0x00;
    unsigned char busy = 0x00;
    
    TRIS_D7=1;
    TRIS_D6=1;
    TRIS_D5=1;
    TRIS_D4=1;
    
    LCD_RW = 1;                      // Set the control bits for read
    LCD_RS = 0;
    
    upperNibble = readNibbleFromLCD();
    OS_delay_ns(LCD_E_CYC);
	//	Read the lower nibble
	lowerNibble = readNibbleFromLCD();
	//	Combine the upper and lower nibbles to produce the full 8-bit byte
	byte = (upperNibble << 4) | lowerNibble;
    busy = byte & 0x80;

   if(busy==0x80)
    {
       OS_delay_ns(LCD_E_CYC);
        return 1;                   // Return TRUE
    }
    else                            // Busy bit is low
    {
        return 0;                   // Return FALSE
    }
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="void LCD_WriteCMD(const unsigned char byte)">
void LCD_WriteCMD(const unsigned char byte)                            
{    
    unsigned char byt,upperNibble,lowerNibble;
    byt = byte;
    upperNibble = byt >> 4;
    lowerNibble = byt & 0x0f;
    
    TRIS_D4=0;
    TRIS_D5=0;
    TRIS_D6=0;
    TRIS_D7=0;
    LCD_RS = 0;                     // Set control bits
    LCD_RW = 0;                     // RS=0 & RW=0 --> Command
    OS_delay_ns(LCD_T_AS);
    writeNibbleToLCD(upperNibble);
    OS_delay_ns(LCD_E_CYC);
    writeNibbleToLCD(lowerNibble);
    OS_delay_ns(LCD_T_AH);
    // Done
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void LCD_WriteByte(const unsigned char byte, const unsigned char adress)">
void LCD_WriteByte(const unsigned char byte, const unsigned char adress)       // Write a byte to CG-RAM address
{
    char upperNibble = byte >> 4;
    char lowerNibble = byte & 0x0f;
    unsigned char cmd = LCD_Command[LCD_CMD_sda] | adress;
    while(LCD_Busy());
    LCD_WriteCMD(cmd);                            // Set CG-RAM adress
    while(LCD_Busy());                                                          // wait for LCD
    LCD_RS = 1;                                                                 // Set control bits:
    LCD_RW = 0;                                                                 // RS=1 & RW=0 --> Write to Ram
    TRIS_D4=0;
    TRIS_D5=0;
    TRIS_D6=0;
    TRIS_D7=0;
    OS_delay_ns(LCD_T_AS);
    writeNibbleToLCD(upperNibble);
    OS_delay_ns(LCD_E_CYC);
    writeNibbleToLCD(lowerNibble);
    OS_delay_ns(LCD_T_AH);
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="void LCD_WriteCGByte(const unsigned char byte, const unsigned char adress)">
void LCD_WriteCGByte(const unsigned char byte, const unsigned char adress)       // Write a byte to CG-RAM address
{
    char upperNibble = byte >> 4;
    char lowerNibble = byte & 0x0f;
    LCD_WriteCMD(LCD_Command[LCD_CMD_sca] | adress);                            // Set CG-RAM adress
    while(LCD_Busy());                                                          // wait for LCD
    LCD_RS = 1;                                                                 // Set control bits:
    LCD_RW = 0;                                                                 // RS=1 & RW=0 --> Write to Ram
    TRIS_D4=0;
    TRIS_D5=0;
    TRIS_D6=0;
    TRIS_D7=0;
    OS_delay_ns(LCD_T_AS);
    writeNibbleToLCD(upperNibble);
    OS_delay_ns(LCD_E_CYC);
    writeNibbleToLCD(lowerNibble);
    OS_delay_ns(LCD_T_AH);
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="void LCD_clr(void)">
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
    while(LCD_Busy());                                                          // wait for LCD
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="void writeNibbleToLCD(char nibble)">

/*
**	Function:		writeNibbleToLCD
**	Parameters:		selectedRegister - either the command or data register
**									   that will be written to
**					nibble - the four bits to be written to the LCD
**	Purpose:		Writes a nibble to the LCD.
**	Returns:		<none>
*/
void writeNibbleToLCD(char nibble)
{
    //	Output the nibble to the LCD
    LCD_EN = 1;
    if(nibble & 0x08) LCD_D7=1; else LCD_D7=0;
    if(nibble & 0x04) LCD_D6=1; else LCD_D6=0;
    if(nibble & 0x02) LCD_D5=1; else LCD_D5=0;
    if(nibble & 0x01) LCD_D4=1; else LCD_D4=0;
    OS_delay_ns(LCD_E_CYC);    //	Toggle the enable line to latch the nibble
    LCD_EN = 0;
    //OS_delay_ns(100);
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="char readNibbleFromLCD()">
/*
**	Function:		readNibbleFromLCD
**	Parameters:		selectedRegister - either the command or data register
**									   that will be read from
**	Purpose:		Read a nibble from the LCD screen.
**	Returns:		nibble - the nibble that is read from the LCD
*/
char readNibbleFromLCD()
{
	//	Create a variable for the nibble
	char nibble = 0x00;
	
	LCD_EN = 1;
	OS_delay_ns(LCD_E_CYC);
    
    if(LCD_D7==1)
    {
        nibble++;
    }
    nibble = nibble << 1;
    if(LCD_D6==1)
    {
        nibble++;
    }
    nibble = nibble << 1;
    if(LCD_D5==1)
    {
        nibble++;
    }
    nibble = nibble << 1;
    if(LCD_D4==1)
    {
        nibble++;
    }
	//	Clear the enable
	LCD_EN = 0;
        //OS_delay_ns(100);

	//	Finally, return the nibble
	return nibble;
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="char LCD_ReadByte(const unsigned char adress)">
/*
**	Function:		char LCD_ReadByte
**	Parameters:		const unsigned int adress - adress to read from
**	Purpose:		Read a byte from the LCD Memory.
**	Returns:		byte - the byte that is read from the LCD
*/
unsigned char LCD_ReadByte(const unsigned char adress)
{
	//	Create variables for the lower and upper nibbles as well as the
	//	entire byte (a combination of the upper/lower nibbles, of course)
	char byte = 0x00;
	char upperNibble = 0x00;
	char lowerNibble = 0x00;
    
    LCD_WriteCMD(LCD_Command[LCD_CMD_sda] | adress);          // Set DD-RAM adress
    while(LCD_Busy());                                        // wait for LCD

    //	Set the data direction for the incoming data
    LCD_RW= 1;
    LCD_RS= 1;
    TRIS_D7=1;
    TRIS_D6=1;
    TRIS_D5=1;
    TRIS_D4=1;

    OS_delay_ns(LCD_E_CYC);
	upperNibble = readNibbleFromLCD();
    OS_delay_ns(LCD_E_CYC);
	//	Read the lower nibble
	lowerNibble = readNibbleFromLCD();
	//	Combine the upper and lower nibbles to produce the full 8-bit byte
	byte = (upperNibble << 4) | lowerNibble;
	//	Clear the read/write strobe
	LCD_RW = 0;
    LCD_RS=0;
	//	Finally, return the byte
	return byte;
}
//</editor-fold>

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
    0b00100000,         // CMD 5    :   Function Set
                        // CMD 5,b2 :   Character font
                        // CMD 5,b3 :   Number of Lines
                        // CMD 5,b4 :   4/8bit Mode
    0b01000000,         // CMD 6    :   Set CG-RAM Address. OR in the address
    0b10000000          // CMD 7    :   Set DD-RAM Address. OR in the address
};

const rom unsigned char LCD_Setting[3] = {
    // Set the right values here, they will be or'ed into the right words, the right time ;)
    0b00000010,         // CMD 2    :   Entry Mode Set
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

#endif /* MOD_Display_LCD_Uni */

