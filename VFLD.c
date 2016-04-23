/*      DevelOS
 * File:    VFLD.c
 * Author:  Steffis PC
 * Destiny: VFLD Display Driver Source
 *
 * Created on 31. März 2013, 20:43
 */

#include "./DevelOS.h"

#if defined MOD_Display_VFLD
// <editor-fold defaultstate="collapsed" desc="void VFLD_WriteByte(int byte, int adress)">
void VFLD_WriteByte(int byte, int adress)
{
    int i,adres;

    VFLD_STB=0;
    OS_delay_ns(400);

    adres = adress | VFLD_Command[1];

    for ( i = 0; i<8; i++ )
    {
        if((adres >> i) & 0x01)
        {
            VFLD_DATA=1;
        }
        else
        {
            VFLD_DATA=0;
        }
        OS_delay_ns(400);
        VFLD_CLK=1;
        OS_delay_ns(400);
        VFLD_CLK=0;
        OS_delay_ns(400);
    }
    
    VFLD_DATA=0;
    VFLD_STB=0;
    
    for ( i = 0; i<8; i++ )
    {
        if((byte >> i) & 0x01)
        {
            VFLD_DATA=1;
        }
        else
        {
            VFLD_DATA=0;
        }
        OS_delay_ns(400);
        VFLD_CLK=1;
        OS_delay_ns(400);
        VFLD_CLK=0;
        OS_delay_ns(400);
    }
    VFLD_DATA=0;
    VFLD_STB=1;
    OS_delay_ns(1000);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_Write(int byte)">
void VFLD_Write(int byte)
{
    int i;
    VFLD_DATA=0;
    VFLD_CLK=0;
    OS_delay_ns(400);
    
    for ( i = 0; i<8; i++ )
    {
        if((byte >> i) & 0x01)
        {
            VFLD_DATA=1;
        }
        else
        {
            VFLD_DATA=0;
        }
        OS_delay_ns(100);
        VFLD_CLK=1;
        OS_delay_ns(400);
        VFLD_CLK=0;
        OS_delay_ns(400);
    }
    VFLD_CLK=1;
    OS_delay_ns(1000);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_Init(void)">
void VFLD_Init(void)
{
    int i=0;
    
    VFLD_STB=0;
    VFLD_Write(VFLD_Command[0]);
    VFLD_STB=1;

    OS_delay_ns(1000);

    VFLD_STB=0;
    VFLD_Write(VFLD_Command[1]);

    for(i=0;i<36;i++)
    {
        OS_delay_ns(1000);
        VFLD_Write(0x00);
    }

    VFLD_STB=1;
    OS_delay_ns(1000);
    VFLD_STB=0;
    VFLD_Write(VFLD_Command[3]);
    VFLD_STB=1;
    OS_delay_ns(1000);
    VFLD_STB=0;
    VFLD_Write(VFLD_Command[2]);
    VFLD_STB=1;
    VFLD_clr();
    OS.DisplayInitialized=1;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char getBitmap(unsigned char code, unsigned char byte)">
unsigned char getBitmap(unsigned char code, unsigned char byte)
{
    if(code<10)                             // Digit
    {
        return VFLD_Bitmaps[code][byte];
    }

    if((code>64) & (code<91))               // Charakter
    {
        return VFLD_Bitmaps[code-55][byte];
    }

    if( (code>47) & (code<58) )             // Digit as Charakter
    {
        return VFLD_Bitmaps[code-48][byte];
    }

    switch(code)                            // All other
    {
        case '+':
            return VFLD_Bitmaps[36][byte];
            break;
        case '-':
            return VFLD_Bitmaps[37][byte];
            break;
        case '*':
            return VFLD_Bitmaps[38][byte];
            break;
        case '/':
            return VFLD_Bitmaps[39][byte];
            break;
        case ',':
            return VFLD_Bitmaps[47][byte];
            break;
        default:
            return 0;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_DoScroll(void)">
void VFLD_DoScroll(void)
{
    unsigned char adres;
    signed char wait;

    if(VFLD.ScrollSpeed != 0)   //Text is waiting
    {
        if(VFLD.ScrollCount<10) // Next Step
        {
            if(VFLD.ScrollSpeed>0)  // go left
            {
                wait=VFLD.ScrollSpeed;
                if(VFLD.ScrollWait>wait)
                {
                    VFLD.String[10]=VFLD.ScrollString[ VFLD.ScrollCount ];
                    for(adres=0;adres<10;adres++)
                    {
                        VFLD.String[adres]=VFLD.String[adres+1];
                    }
                    VFLD.ScrollWait=0;
                    VFLD.ScrollCount++;
                }
                else
                {
                    VFLD.ScrollWait++;
                }
            }
            else                    // go right
            {
                wait = VFLD.ScrollSpeed * (-1);
                if(VFLD.ScrollWait>wait)
                {
                    for(adres=0;adres<10;adres++)
                    {
                        VFLD.String[10-adres]=VFLD.String[9-adres];
                    }
                    VFLD.String[0]=VFLD.ScrollString[ 9-VFLD.ScrollCount ];
                    VFLD.ScrollWait=0;
                    VFLD.ScrollCount++;
                }
                else
                {
                    VFLD.ScrollWait++;
                }
            }
        }
        else                    // Done
        {
            VFLD.ScrollWait=0;
            VFLD.ScrollCount=0;
            VFLD.ScrollSpeed=0;
        }
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_Refresh(void)">
void VFLD_Refresh(void)
{
    unsigned char segment,symbol;
    
    VFLD_DoScroll();

    for(segment=1;segment<11;segment++) // Render Text first
    {
        VFLD.Buffer[segment][0]=VFLD.Buffer[segment][0] | getBitmap(VFLD.String[10-segment],0);
        VFLD.Buffer[segment][1]=VFLD.Buffer[segment][1] | getBitmap(VFLD.String[10-segment],1);
    }

    for(symbol=0;symbol<22;symbol++)    // OR in the Symbols
    {
        if(VFLD.Symbol[symbol]>0)
        {
            switch(symbol)
            {
                case 0:     // Segment 0, Low-Byte, Bit 3 : Ring
                    VFLD.Buffer[0][0] = VFLD.Buffer[0][0] | 0b00001000;
                    break;
                case 1:     // Segment 0, Low-Byte, Bit 4 : *r* Innen
                    VFLD.Buffer[0][0] = VFLD.Buffer[0][0] | 0b00010000;
                    break;
                case 2:     // Segment 0, Low-Byte, Bit 5 : *g* Innen
                    VFLD.Buffer[0][0] = VFLD.Buffer[0][0] | 0b00100000;
                    break;
                case 3:     // Segment 0, Low-Byte, Bit 6 : *b* Innen
                    VFLD.Buffer[0][0] = VFLD.Buffer[0][0] | 0b01000000;
                    break;
                case 4:     // Segment 1, Low-Byte, Bit 0 : *r* REC
                    VFLD.Buffer[1][0] = VFLD.Buffer[1][0] | 0b00000001;
                    break;
                case 5:     // Segment 2, Low-Byte, Bit 0 : PROG
                    VFLD.Buffer[2][0] = VFLD.Buffer[2][0] | 0b00000001;
                    break;
                case 6:     // Segment 2, Low-Byte, Bit 1 : RAND
                    VFLD.Buffer[2][0] = VFLD.Buffer[2][0] | 0b00000010;
                    break;
                case 7:     // Segment 3, Low-Byte, Bit 0 : Komma
                    VFLD.Buffer[3][0] = VFLD.Buffer[3][0] | 0b00000001;
                    break;
                case 8:     // Segment 3, Low-Byte, Bit 1 : Doppelpunkt
                    VFLD.Buffer[3][0] = VFLD.Buffer[3][0] | 0b00000010;
                    break;
                case 9:     // Segment 4, Low-Byte, Bit 0 : ANGLE
                    VFLD.Buffer[4][0] = VFLD.Buffer[4][0] | 0b00000001;
                    break;
                case 10:    // Segment 4, Low-Byte, Bit 1 : ZOOM
                    VFLD.Buffer[4][0] = VFLD.Buffer[4][0] | 0b00000010;
                    break;
                case 11:    // Segment 5, Low-Byte, Bit 0 : PBC
                    VFLD.Buffer[5][0] = VFLD.Buffer[5][0] | 0b00000001;
                    break;
                case 12:    // Segment 5, Low-Byte, Bit 1 : Doppelpunkt
                    VFLD.Buffer[5][0] = VFLD.Buffer[5][0] | 0b00000010;
                    break;
                case 13:    // Segment 0, Low-Byte, Bit 7 : Stop
                    VFLD.Buffer[0][0] = VFLD.Buffer[0][0] | 0b10000000;
                    break;
                case 14:    // Segment 0, High-Byte, Bit 0 : Pause
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b00000001;
                    break;
                case 15:    // Segment 0, High-Byte, Bit 1 : Play
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b00000010;
                    break;
                case 16:    // Segment 0, High-Byte, Bit 2 : /R
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b00000100;
                    break;
                case 17:    // Segment 0, High-Byte, Bit 3 : L
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b00001000;
                    break;
                case 18:    // Segment 0, High-Byte, Bit 4 : PCM
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b00010000;
                    break;
                case 19:    // Segment 0, High-Byte, Bit 5 : *r* dts
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b00100000;
                    break;
                case 20:    // Segment 0, High-Byte, Bit 6 : *r* MIC
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b01000000;
                    break;
                case 21:   // Segment 0, High-Byte, Bit 7 : Dolby Digital
                    VFLD.Buffer[0][1] = VFLD.Buffer[0][1] | 0b10000000;
                    break;
            }
        }
    }

    // Now, finally write the Data to the Display
    VFLD_WriteBuffer(3);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="char VFLD_WriteBuffer(char mode)">
char VFLD_WriteBuffer(char mode)
{
    char segment,adres;

    switch(mode)
    {
        case 1:         // old-style, each byte adressed separately
            for(segment=0;segment<12;segment++)         // Now, finally write the Data to the Display
            {
                adres=( segment * 0x03 );
                VFLD_WriteByte(VFLD.Buffer[segment][0], adres);
                VFLD.Buffer[segment][0]=0;
                VFLD_WriteByte(VFLD.Buffer[segment][1], (adres+1));
                VFLD.Buffer[segment][1]=0;
            }
            break;
        case 2:         // try it in one pass with control byte
            VFLD_STB=0;
            VFLD_Write(VFLD_Command[0]);    // Data Set
            VFLD_STB=1;
            OS_delay_ns(1000);
            VFLD_STB=0;
            VFLD_Write(VFLD_Command[1]);    // Set Adress 0
            OS_delay_ns(1000);
            for(segment=0;segment<12;segment++)             // for each adress
            {
                VFLD_Write(VFLD.Buffer[segment][0]);        // 1 low byte
                VFLD.Buffer[segment][0]=0;
                OS_delay_ns(1000);
                VFLD_Write(VFLD.Buffer[segment][1]);        // 1 high byte
                VFLD.Buffer[segment][1]=0;
                OS_delay_ns(1000);
                VFLD_Write(0x00);                           // 1 dummy byte
                OS_delay_ns(1000);
            }
            VFLD_STB=1;
            OS_delay_ns(1000);
            VFLD_STB=0;
            VFLD_Write(VFLD_Command[3]);    // Set Mode
            VFLD_STB=1;
            OS_delay_ns(1000);
            VFLD_STB=0;
            if(VFLD.Duty<16 & VFLD.Duty>0)                  // if value valid
            {
                VFLD_Write(VFLD_Command[2] + VFLD.Duty);    // use duty value
            }
            else
            {
                VFLD_Write(VFLD_Command[2]);                // control command
            }
            VFLD_STB=1;
            break;
        case 3:         // old-style, with control byte
            for(segment=0;segment<12;segment++)         // Now, finally write the Data to the Display
            {
                adres=( segment * 0x03 );
                VFLD_WriteByte(VFLD.Buffer[segment][0], adres);
                VFLD.Buffer[segment][0]=0;
                VFLD_WriteByte(VFLD.Buffer[segment][1], (adres+1));
                VFLD.Buffer[segment][1]=0;
            }
            OS_delay_ns(1000);
            VFLD_STB=0;
            if(VFLD.Duty<16 & VFLD.Duty>0)                  // if value valid
            {
                VFLD_Write(VFLD_Command[2] + VFLD.Duty);    // use duty value
            }
            else
            {
                VFLD_Write(VFLD_Command[2]);                // control command
            }
            VFLD_STB=1;
            break;
        default:        // invalid
            return 0;
            break;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_Value(unsigned int value)">
void VFLD_Value(unsigned int value)
{
    char digit,trial;
    trial=0;

    if(value>9999)
    {
        value=9999;
    }

    digit=0;
    while(value>=1000)
    {
        digit++;
        value -= 1000;
    }
    if(digit>0)
    {
        VFLD.String[6]=digit;
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
        VFLD.String[7]=digit;
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
        VFLD.String[8]=digit;
        trial=1;
    }

    VFLD.String[9]=value;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_WriteByte(int byte, int adress)">
void VFLD_Time(char hour, char mins, char secs)
{
    char digit;
    VFLD.String[0]=' ';
    VFLD.String[1]=' ';
    VFLD.String[2]=' ';
    VFLD.String[3]=' ';
    digit=0;
    while(hour>=10)
    {
        hour -= 10;
        digit++;
    }
    VFLD.String[4]=digit;
    VFLD.String[5]=hour;

    digit=0;
    while(mins>=10)
    {
        mins -= 10;
        digit++;
    }
    VFLD.String[6]=digit;
    VFLD.String[7]=mins;

    digit=0;
    while(secs>=10)
    {
        secs -= 10;
        digit++;
    }
    VFLD.String[8]=digit;
    VFLD.String[9]=secs;
    digit=0;

    VFLD.Symbol[8]=1;
    VFLD.Symbol[12]=1;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_print(const unsigned char* string[])">
void VFLD_print(const unsigned char* string[])
{
    strcpypgm2ram( &VFLD.String, string);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_scroll(const unsigned char* string[], signed char speed)">
void VFLD_scroll(const unsigned char* string[], signed char speed)
{
    strcpypgm2ram( &VFLD.ScrollString, string);
    VFLD.ScrollCount=0;
    VFLD.ScrollWait=0;
    VFLD.ScrollSpeed=speed;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_WriteByte(int byte, int adress)">
void VFLD_clr(void)
{
    char i;

    VFLD.ScrollCount=0;
    VFLD.ScrollSpeed=0;
    VFLD.ScrollWait=0;
    VFLD.Duty=OS.DisplayBrightness;

    for(i=0;i<10;i++)
    {
        VFLD.String[i]=' ';
        VFLD.ScrollString[i]=' ';
    }

    for(i=0;i<22;i++)
    {
        VFLD.Symbol[i]=0;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void VFLD_Init(void) ???">
//void VFLD_Init(void)
/*{
    int i,adres;

    VFLD_STB=0;
    OS_delay_ns(400);

    adres = adress | VFLD_Command[1];

    for ( i = 0; i<8; i++ )
    {
        if((adres >> i) & 0x01)
        {
            VFLD_DATA=1;
        }
        else
        {
            VFLD_DATA=0;
        }
        OS_delay_ns(400);
        VFLD_CLK=1;
        OS_delay_ns(400);
        VFLD_CLK=0;
        OS_delay_ns(400);
    }

    VFLD_DATA=0;
    VFLD_STB=0;

    for ( i = 0; i<8; i++ )
    {
        if((byte >> i) & 0x01)
        {
            VFLD_DATA=1;
        }
        else
        {
            VFLD_DATA=0;
        }
        OS_delay_ns(400);
        VFLD_CLK=1;
        OS_delay_ns(400);
        VFLD_CLK=0;
        OS_delay_ns(400);
    }
    VFLD_DATA=0;
    VFLD_STB=1;
    OS_delay_ns(1000);
}*/// </editor-fold>

#pragma romdata VFLD_ROM    // Display ROM Bitmaps
const rom unsigned char VFLD_Bitmaps[48][2]= {
    {0b10001100,0b11000100},    // 0
    {0b10000000,0b01000000},    // 1
    {0b00001100,0b11000011},    // 2
    {0b10000100,0b11000011},    // 3
    {0b10000000,0b01000111},    // 4
    {0b10000100,0b10000111},    // 5
    {0b10001100,0b10000111},    // 6
    {0b10000000,0b11000000},    // 7
    {0b10001100,0b11000111},    // 8
    {0b10000100,0b11000111},    // 9
    {0b10001000,0b11000111},    // A
    {0b10100100,0b11010001},    // B
    {0b00001100,0b10000100},    // C
    {0b10100100,0b11010000},    // D
    {0b00001100,0b10000110},    // E
    {0b00001000,0b10000110},    // F
    {0b10001100,0b10000101},    // G
    {0b10001000,0b01000111},    // H
    {0b00100100,0b10010000},    // I
    {0b10000100,0b01000000},    // J
    {0b01001000,0b00100110},    // K
    {0b00001100,0b00000100},    // L
    {0b10001000,0b01101100},    // M
    {0b11001000,0b01001100},    // N
    {0b10001100,0b11000100},    // O
    {0b00001000,0b11000111},    // P
    {0b11001100,0b11000100},    // Q
    {0b01001000,0b11000111},    // R
    {0b10000100,0b10000111},    // S
    {0b00100000,0b10010000},    // T
    {0b10001100,0b01000100},    // U
    {0b10001100,0b01000100},    // V
    {0b10101100,0b01000100},    // W
    {0b01010000,0b00101000},    // X
    {0b00100000,0b00101000},    // Y
    {0b00010100,0b10100000},    // Z
    {0b00100000,0b00010011},    // +
    {0b00000000,0b00000011},    // -
    {0b01110000,0b00111011},    // *
    {0b00010000,0b00100000},    // /
    {0b01000000,0b00001000},    // Backslash
    {0b00001000,0b00000100},    // | Left
    {0b00100000,0b00010000},    // | Middle
    {0b10000000,0b01000000},    // | Right
    {0b10101000,0b01010100},    // |||
    {0b00101000,0b00010100},    // || Left
    {0b10100000,0b01010000},    // || Right
    {0b00010000,0b00000000}     // ,
};
const rom unsigned char VFLD_Command[4] = {
    0b01000000,     // Data Set Command
    0b11000000,     // Adress Set Command (OR-in 5-bit-adress)
    0b10001000,     // Display Control command. OR-in 3-bit duty-value. bit 3 = display on/off bit
    0b00001011      // Mode set command
};

#endif /* MOD_Display_VFLD */
