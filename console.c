#include "DevelOS.h"

void c_cr(void)
{
    char i;

    // set cursor home
    console.cursor_x=0;
    console.cursor_y++;

    if(console.cursor_y >= CON_lines)
    {
        console.cursor_y=CON_lines-1;
        // shift Display Buffer 1 line
        for(i=0; i < CON_lines-1 ;i++)
        {
            strncpy( console.Buffer[i], console.Buffer[i+1], CON_width );
        }   
        // clear the new line
        memset(console.Buffer[CON_lines-1], ' ', CON_width );
    }
}

void c_pos(const unsigned char x, const unsigned char y)
{
    if(x<CON_width)
    {
        console.cursor_x=x;
    }
    if(y<CON_lines)
    {
        console.cursor_y=y;
    }
}

void c_print(const char* string[])
{
    char len,byte;
    char buff[CON_width];
    len=0;

    for(byte=0;byte<CON_width;byte++)
    {
        buff[byte]=0;
    }
    byte=0;
        
    strcpypgm2ram( &buff, string ); //
    byte=buff[0];
    
    while(byte != 0)
    {
        // wrap at end of line
        if(console.cursor_x >= CON_width)
        {
            d_cr();
        }
        
        // write byte to console buffer
        console.Buffer[console.cursor_y][console.cursor_x++]=byte;

        //read next byte from buffer
        len++;
        byte = buff[len];

        if(byte=='\0')          // on 0x00: string terminated
        {
            byte=0;
        }
        else if(byte=='\n')     // on newline: do carriage return, also terminate string
        {
            d_cr();
            byte=0;
        }
        else if(byte=='\r')     // on return: do carriage return, continue string
        {
            d_cr();
        }
    }
}

void c_clr(void)
{
    char y;
    // first, init the internal variables:
    for(y=0;y<CON_lines;y++)
    {
        memset(console.Buffer[y], ' ', CON_width );
    }
    
    console.cursor_x=0;
    console.cursor_y=0;
}

void c_value(const unsigned int val)
{
    unsigned char len;
    unsigned int value;
    unsigned char buff[5];
    value = val;
    memset(buff, '\0', sizeof( buff ));
    len=0;
  
    sprintf( buff, "%d", value);
    len = strlen(buff);
    for(value=0;value<len;value++)
    {
        console.Buffer[console.cursor_y][console.cursor_x] = buff[value];
        console.cursor_x++;
    }
    //strncpy( , &buff, len );
    /*for(digit=0;digit<CON_width;digit++)
    {
        buff[digit] = ' ';
    }
    
    trial=0;
    digit=0;
    len=0;
    value=val;

    while(value>=10000)
    {
        digit++;
        value -= 10000;
    }
    if(digit>0)
    {
        buff[len] = (unsigned char) 48+digit;
        len++;
        //console.Buffer[console.cursor_y][console.cursor_x++]=(unsigned char) 48+digit;
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
        buff[len] = (unsigned char) 48+digit;
        len++;
        //console.Buffer[console.cursor_y][console.cursor_x++]=(unsigned char) 48+digit;
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
        buff[len] = (unsigned char) 48+digit;
        len++;
        //console.Buffer[console.cursor_y][console.cursor_x++]=(unsigned char) 48+digit;
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
        buff[len] = (unsigned char) 48+digit;
        len++;
        //console.Buffer[console.cursor_y][console.cursor_x++]=(unsigned char) 48+digit;
        trial=1;
    }
    buff[len] = (unsigned char) 48+digit;
    
    if( (console.cursor_x+len) < CON_width )
    {
        strncpy( &console.Buffer[console.cursor_y][console.cursor_x], buff[0], CON_width );
    }*/
}

