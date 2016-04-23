#include "./DevelOS.h"

#ifdef MOD_keypad

// <editor-fold defaultstate="collapsed" desc="void InitKeypad(void)">
void InitKeypad(void)
{
    char i;

    for(i=0;i<Keynum;i++)
    {
        KP.Key[i]=0;
        KP.Matrix[i].limit0=1023;
        KP.Matrix[i].limit1=1023;
        KP.Matrix[i].line=0;
    }

    // Scanlines for Keypad
    Scan1_tr = 0;
    Scan2_tr = 0;
    Scan3_tr = 0;

    Scan1 =0;
    Scan2 =0;
    Scan3 =0;

}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="char ScanKeys(void)">
char ScanKeys(void) // Detect Keypress on Matrix Keypad or clear key buffer
{
    char num,event;
    event=0;

    for(num=0;num<Keynum;num++)
    {
        if( (KP.Scanline[ KP.Matrix[num].line ] >= KP.Matrix[num].limit0) && 
                (KP.Scanline[ KP.Matrix[num].line ] <= KP.Matrix[num].limit1) ) // Key is pressed
        {
            KP.Scanline[ KP.Matrix[num].line ]=0;                               // clear the scanline-buffer
            KP.Key[num]=1;                                                      // we have a press-event
        }
        else                                                                    // Key is unpressed
        {
            if(KP.Key[num]>0)                                                   // Key was pressed before
            {
                KP.Key[num]=-1;                                                 // we have an unpress-event
            }
        }
    }
    return event;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="char SetKeylimit(char code, char line, unsigned int value, char tol)">
char SetKeylimit(char code, char line, unsigned int value, char tol)
{
    unsigned int t;
    t=((value*tol)/100);
    KP.Matrix[code].limit0   = value-t;
    KP.Matrix[code].limit1   = value+t;
    KP.Matrix[code].line     = line;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void ScanKeypad(void)">
void ScanKeypad(void)
{
    ADC_Data.count=33;
    ADCON0bits.CHS=2;
    ADCON0bits.GO=1;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char LoadEEPROM_KP(unsigned char block)">
unsigned char LoadEEPROM_KP(unsigned char block)   // Read Keypad Data from EEPROM
{
    unsigned char temp[5];
    unsigned char num,hbyte0,lbyte0,hbyte1,lbyte1,adr,bytes;

    bytes=0;

    for(num=0;num<Keynum;num++)
    {
        adr =( (num * 5) ) ;
        EE_read_block(eprom.Block[block].adress+adr, &temp[0], 5);
        bytes +=5;
        hbyte0=temp[1];
        lbyte0=temp[2];
        hbyte1=temp[3];
        lbyte1=temp[4];
        KP.Matrix[num].limit0 = lbyte0;
        KP.Matrix[num].limit0 += hbyte0*256;
        KP.Matrix[num].limit1 = lbyte1;
        KP.Matrix[num].limit1 += hbyte1*256;
        KP.Matrix[num].line = temp[0];
    }
    return bytes;   // block loaded
}// </editor-fold>

//<editor-fold defaultstate="collapsed" desc="unsigned char SaveEEPROM_KP(unsigned char block)">
unsigned char SaveEEPROM_KP(unsigned char block)        // Write Keypad Data to EEPROM
{
    unsigned char temp[5];
    unsigned char num,used,hbyte0,lbyte0,hbyte1,lbyte1,adr;
    used=0;

    temp[0]=0xAA;
    temp[1]=0xAA;
    temp[2]=0xAA;
    temp[3]=0xAA;
    temp[4]=0xAA;

    // Clean the Block first
    for(num=0;num<32;num++)
    {
        EE_write_block(eprom.Block[block].adress + (num*4), &temp[0], 4);
    }

    for(num=0;num<Keynum;num++)
    {
        hbyte0=0;
        lbyte0=0;
        hbyte1=0;
        lbyte1=0;
        adr=( (num * 5) ); //  5 bytes data

        hbyte0=KP.Matrix[num].limit0/256;
        lbyte0=KP.Matrix[num].limit0-hbyte0;
        hbyte1=KP.Matrix[num].limit1/256;
        lbyte1=KP.Matrix[num].limit1-hbyte0;

        temp[0]=KP.Matrix[num].line;
        temp[1]=hbyte0;
        temp[2]=lbyte0;
        temp[3]=hbyte1;
        temp[4]=lbyte1;

        EE_write_block(eprom.Block[block].adress + adr, &temp[0], 5);
        used +=5;
        if(used >= 120)
        {
            temp[0]=used;
            temp[1]=EE_sig_Keypad;
            EE_write_block(eprom.Block[block].adress + (EE_Blocksize-2), &temp[0], 2);
            used=0;
            block++;
        }
        temp[0]=used;
        temp[1]=EE_sig_Keypad;
        EE_write_block(eprom.Block[block].adress + (EE_Blocksize-2), &temp[0], 2);
    }
    return used;// success
}

//</editor-fold>

#endif

