/*      DevelOS
 *
 * File:    ps2-keyboard.h
 * Module:  MOD_Input_KB_PS2
 * Author:  Steffis PC
 * Destiny: PS2-Keyboard Driver
 *
 * Created on 27. April 2013, 21:16
 */

#include "./DevelOS.h"

#ifdef MOD_Input_KB_PS2
#include "ps2-keyboard.h"

void InitKB_PS2(void)
{
    Keyboard.RxBuff[0]=0;
    Keyboard.RxBuff[1]=0;
    Keyboard.RxBuff[2]=0;
    Keyboard.RxBuff[3]=0;
    Keyboard.TxBuff[0]=0;
    Keyboard.TxBuff[1]=0;
    Keyboard.BitBuffer=0;
    Keyboard.BitCounter=0;
    Keyboard.ByteCounter=0;
    Keyboard.isExtended=0;
    Keyboard.isBreak=0;
}

unsigned int getKeyCode(unsigned int ScanCode)
{
    unsigned char i,code;
    code=ScanCode;

    for(i=0;i<104;i++)
    {
        if(PS2_Codes[0][i]==code)
        {
            if(Keyboard.Shift==1)
            {
                return PS2_Codes[1][i];
            }
            else
            {
                return PS2_Codes[2][i];
            }
            
        }
    }
}

void PS2_Byte(unsigned int byte)
{
    if(byte==0xE0)
    {
        Keyboard.isExtended=1;
        return;
    }

    if(byte==0xF0)
    {
        Keyboard.isBreak=1;
        return;
    }

    if(Keyboard.isBreak)
    {
        addEvent( EV_ScanBreak, byte );
        Keyboard.isExtended=0;
        Keyboard.isBreak=0;
    }
    else
    {
        addEvent( EV_ScanMake, byte );
        Keyboard.isExtended=0;
        Keyboard.isBreak=0;
    }
}

const rom char PS2_Codes[3][104] = {
    {0x1C,0x32,0x21,0x23,0x24,0x2B,0x34,0x33,0x43,0x3B,0x42,0x4B,0x3A,0x31,0x44,0x4D,0x15,0x2D,0x1B,0x2C,
     0x3C,0x2A,0x1D,0x22,0x1A,0x35,0x45,0x16,0x1E,0x26,0x25,0x2E,0x36,0x3D,0x3E,0x46,0x29,0x54,0x5B,0x4C,
     0x52,0x41,0x49,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x76,0x5A},

    {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
     'U','V','W','X','Y','Z','=','!','"','§','$','%','&','/','(',')',' ','Ü','*','Ö',
     'Ä',';',':',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x27,0x13},

    {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t',
     'u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9',' ','ü','+','ö',
     'ä',',','.',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x27,0x13}
};

#endif

