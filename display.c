#include "./DevelOS.h"

#ifdef MOD_Display

void InitDisplay(void)
{
    // now, the hardware:

    #if defined MOD_Display_VFLD
        VFLD_STB_TRIS =0;
        VFLD_DIN_TRIS =0;
        VFLD_CLK_TRIS =0;
        VFLD_STB =1;            // STB is low-active
        VFLD_CLK =1;            // CLK should be 1 in Idle State
        VFLD_DATA =0;
        VFLD_Init();
        d_print( "VFL\n\0" );
    #elif defined MOD_Display_LCD
        InitLCD();
        Display.view_y=LCD.Dimensions.height-1;     // wir sehen immer die untersten zeilen
        d_print( "LCD\n\0" );
    #elif defined MOD_Display_LCD_Uni
        while(InitLCD())
        {
            OS_delay_ns(LCD_E_CYC*2);
        }
        Display.view_y=LCD.Dimensions.height-1;     // wir sehen immer die untersten zeilen
        d_print( "LCDuni\n\0" );
    #endif
    // alles bereit, ready to go    
    OS.DisplayInitialized=1;
}

void d_cr(void)
{
    char i;

    // shift Display Buffer 1 line
    for(i=BufferLines-1;i>0;i--)
    {
        strncpy( &Display.Buffer[i], Display.Buffer[i-1], 20 );
    }
    
    // clear the new line
    strncpypgm2ram( &Display.Buffer[0], "                    ", 20 );
    
    // set cursor home
    Display.cursor_x=0;
    Display.cursor_y=0;
}

void d_pos(const unsigned char x, const unsigned char y)
{
    if(x<20)
    {
        Display.cursor_x=x;
    }
    if(y<BufferLines)
    {
        Display.cursor_y=y;
    }
}

void d_print(const char* string[])
{
    char len,byte;
    char buff[20];
    len=0;
    byte=0;

    strcpypgm2ram( &buff, string ); //
    byte=buff[0];
    while(byte != 0)
    {
        // wrap at end of line
        if(Display.cursor_x>19)
        {
            d_cr();
        }
        
        // write byte to display buffer
        Display.Buffer[Display.cursor_y][Display.cursor_x]=byte;
        
        // increment cursor
        Display.cursor_x++;
        len++;
        
        //read next byte from buffer
        byte=buff[len];
        if(byte=='\0')          // on 0x00: string terminated
        {
            byte=0;
        }
        else if(byte=='\n')     // on newline: do carriage return, also terminate string for now
        {
            d_cr();
            byte=0;
        }
    }
}

void d_clr(void)
{
    char x,y;
    // first, init the internal variables:
    for(y=0;y<BufferLines;y++)
    {
        for(x=0;x<20;x++)
        {
            Display.Buffer[y][x]=' ';
        }
    }
    Display.cursor_x=0;
    Display.cursor_y=0;
}

void d_value(const unsigned int val)
{
    unsigned int value;
    unsigned char digit,trial;
    trial=0;
    digit=0;
    value=val;

    while(value>=10000)
    {
        digit++;
        value -= 10000;
    }
    if(digit>0)
    {
        Display.Buffer[Display.cursor_y][Display.cursor_x++]=(unsigned char) 48+digit;
        trial=1;
    }
    digit = 0;

    while(value>=1000)
    {
        digit++;
        value -= 1000;
    }
    if(digit>0)
    {
        Display.Buffer[Display.cursor_y][Display.cursor_x++]=(unsigned char) 48+digit;
        trial=1;
    }
    digit = 0;

    while(value>=100)
    {
        digit++;
        value -= 100;
    }
    if( (digit>0) | trial>0)
    {
        Display.Buffer[Display.cursor_y][Display.cursor_x++]=(unsigned char) 48+digit;
        trial=1;
    }
    digit=0;

    while(value>=10)
    {
        digit++;
        value -= 10;
    }
    if( (digit>0) | trial>0)
    {
        Display.Buffer[Display.cursor_y][Display.cursor_x++]=(unsigned char) 48+digit;
        trial=1;
    }

    Display.Buffer[Display.cursor_y][Display.cursor_x++]=(unsigned char) 48+value;
}

void RefreshDisplay(void)
{
    char x,y;

    #if defined MOD_Display_VFLD
        VFLD_Refresh();
    #elif defined MOD_Display_LCD
        while(LCD.Busy == 1);       // LCD is Busy, do not corrupt the Buffer
        
        for(y=0;y<4;y++)
        {
            for(x=0;x<20;x++)
            {
                LCD.Buffer[y][x]=Display.Buffer[Display.view_y-y][Display.view_x+x];        
                // TODO: Display.view_x should always be 0 for now, this is untested
            }
        }
        RefreshLCD();
    #elif defined MOD_Display_LCD_Uni
        if(LCD.Busy == 0)
        {
            for(y=0;y<LCDuni_Lines;y++)
            {
                for(x=0;x<LCDuni_Cols;x++)
                {
                    LCD.Buffer[y][x]=Display.Buffer[Display.view_y-y][Display.view_x+x];        
                    //LCD.Buffer[y][x]=Display.Buffer[y][x];        
                    // TODO: Display.view_x should always be 0 for now, this is untested
                }
            }
            RefreshLCD();
        }
    #endif /* MOD_Display_<Module> */
}

#endif /* MOD_Display */
