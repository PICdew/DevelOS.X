/*      DevelOS
 * File:    newmain.c
 * Author:  Steffis PC
 * Destiny: Main source for DevelOS
 *
 * Created on 31. März 2013, 20:43
 */

#include "./main.h"
#include "./configbits.h"
#include "./DevelOS.h"
#define EE_1k

/*
 * Now, we declare the global Variables:
 */

// <editor-fold defaultstate="collapsed" desc="Global Variable Definitions">
//#pragma udata OS_Counters
//volatile struct Counters T0counters;
#pragma udata OS_Data
struct OS_State OS;
#pragma udata AD_Data
volatile struct ADC_State ADC_Data;
#pragma udata ISR_Count
volatile struct Counter isr_lf_count[ISR_LF_Count], isr_hf_count[ISR_HF_Count];
#pragma udata
#if defined MOD_Display
    #pragma udata TextBuffer
    struct Disp Display;
    #pragma udata HardBuffer

    #if defined MOD_Display_VFLD
        struct VF_Display VFLD;
    #elif defined MOD_Display_LCD
        struct LCD_Display LCD;
    #elif defined MOD_Display_LCD_Uni
        struct LCD_Display LCD;
    #endif /* MOD_Display_<*> */
    #pragma udata
#endif /* MOD_Display */

#if defined MOD_keypad
    #pragma udata KP_Data
    struct Keypad KP;
    #pragma udata
#endif /* MOD_keypad */

#if defined MOD_rtc
    #pragma udata RTC
    struct RTC_Time rtc;
    #pragma udata
#endif /* MOD_rtc */

#if defined MOD_FlashFS
    #pragma udata Flash
    struct Flash_Data Flash;
    //struct EEPROM eprom;
    #pragma udata
#endif /* MOD_FlashFS */

#if defined MOD_Input_KB_PS2
    volatile struct KB_PS2 Keyboard;
#endif /* MOD_Input_KB_PS2 */

#ifdef MOD_I2C
    #pragma udata i2c
    struct I2C_BUS i2c_bus;
    #pragma udata
#endif /* MOD_I2C */

#ifdef MOD_FlashFS_extI2C
    #pragma udata i2c
    struct I2Ceeprom I2C_eprom;
    #pragma udata
#endif

// </editor-fold>

/*
 * Here come the service routines for IRQs:
 */
// <editor-fold defaultstate="collapsed" desc="Interrupt Service Routines">
#pragma code high_vector=0x08
void high_vector (void)
{
    _asm GOTO High_ISR _endasm
}
#pragma code
#pragma interrupt High_ISR nosave=section("ISR_Count")
void High_ISR(void)         
{
    char i;

    if(INTCONbits.TMR0IF)       // <editor-fold defaultstate="collapsed" desc="Timer 0 (HF Timer)">
    {
        INTCONbits.TMR0IF = 0;      //irq clear
        for(i=0; i<ISR_HF_Count;i++)
        {
            if(isr_hf_count[i].Count++ > isr_hf_count[i].Wait)
            {
                if(OS.isInitialized==1)
                {
                    addEvent(EV_HF_Timer, i);
                }
                isr_hf_count[i].Count=0;
            }
        }

        INTCONbits.TMR0IE = 1;      //irq enable
    } // </editor-fold>

    if(PIR1bits.TMR1IF)         // <editor-fold defaultstate="collapsed" desc="Timer 1 (LF Timer)">
    {
        PIR1bits.TMR1IF=0;
        for(i=0; i<ISR_LF_Count;i++)
        {
            if(isr_lf_count[i].Count++ > isr_lf_count[i].Wait)
            {
                if(OS.isInitialized==1)
                {
                    addEvent(EV_LF_Timer, i);
                }
                isr_lf_count[i].Count=0;
            }
        }

        PIE1bits.TMR1IE = 1;    
    } // </editor-fold>
    
    if(INTCONbits.INT0IF)       // <editor-fold defaultstate="collapsed" desc="Ext INT 0">
    {
        INTCONbits.INT0IF=0;
        // Bit einlesen & zählen
#ifdef MOD_Input_KB_PS2
        if(PS2_Data==1)
        {
            Keyboard.BitBuffer = Keyboard.BitBuffer | 0b1000000000000000;
        }
        else
        {
            Keyboard.BitBuffer = Keyboard.BitBuffer & 0b0111111111111111;
        }
        Keyboard.BitCounter++;
        
        if(Keyboard.BitCounter==11)                             // nach 11 bit ist der frame vollständig
        {
            Keyboard.BitBuffer = Keyboard.BitBuffer >> 5;       // nochmal 5 nach rechts schieben
            PS2_Byte(Keyboard.BitBuffer & 0x00FF);              // lower-byte extrahieren
            Keyboard.BitBuffer=0;
            Keyboard.BitCounter=0;
        }
        else if(Keyboard.BitCounter<11)                         // weiterschieben
        {
            Keyboard.BitBuffer = Keyboard.BitBuffer>>1;
        }
        else                                                    // fehler, reset
        {
            Keyboard.BitBuffer=0;
            Keyboard.BitCounter=0;
        }
#endif
        INTCONbits.INT0IE=1;
    } // </editor-fold>

    INTCONbits.GIEH =1;
}
#pragma code low_vector=0x18
void low_vector (void)
{
    _asm GOTO Low_ISR _endasm
}
#pragma code
#pragma interruptlow Low_ISR 
void Low_ISR(void)     
{
    unsigned int temp;
    unsigned char tmp;
    
    if(PIR1bits.ADIF)           // <editor-fold defaultstate="collapsed" desc="ADC Module">
    {
        PIR1bits.ADIF=0;                        //  Clear IRQ
        
        temp=ADRESL;
        temp+=ADRESH*256;

        PIE1bits.ADIE=1;                        //  Enable IRQ
    } // </editor-fold>
    
    INTCONbits.GIEL =1;
}
// </editor-fold>


/*
 * Time to do something!
 */

#pragma code

void main(void)
{
    // temp variables for the main() loop must be defined here
    unsigned char i=0;

    // First of all, look if this was an reset, and why
    HandleReset();

    // Then, initialize global variables
    InitGlobals();
    
    // Now, Init the Chip Hardware
    InitChip();
    
    // Init OS to sane values
    InitOS();

    // Display Init
    InitDisplay();
    
    
    RefreshDisplay();
    #ifdef BOOT_SLOW 
    OS_delay_ns(10000000);
    #endif
    
    d_cr();
    d_print("DevelOS-16 v0.1\0");
    RefreshDisplay();
    #ifdef BOOT_SLOW
    OS_delay_ns(5000000);
    #endif
    
#ifdef MOD_FlashFS
    // Init FlashFS
    d_cr();
    d_print("FlashFS\0");
    RefreshDisplay();
    #ifdef BOOT_SLOW
    OS_delay_ns(5000000);
    #endif
    InitFlash();
    RefreshDisplay();
    #ifdef BOOT_SLOW
    OS_delay_ns(5000000);
    #endif /* BOOT_SLOW */
    
    // Load EEPROM Data
    for(i=0;i<EE_Blocks;i++)
    {
        switch(Flash.eprom.Block[i].signature)
        {
            case EE_sig_FlashFS:
                // TODO: use this block somehow
                // it should store info on the other blocks,
                // checksums, and info on external eeprom chips
                d_cr();
                d_print("FlashFS at \0");
                d_value(i);
                RefreshDisplay();        
                #ifdef BOOT_SLOW
                OS_delay_ns(5000000);
                #endif /* BOOT_SLOW */
                break;
            case EE_sig_System:
                // This holds all the static settings for the OS
                d_cr();
                d_print("DevelOS at \0");
                d_value(i);
                RefreshDisplay();     
                #ifdef BOOT_SLOW
                OS_delay_ns(5000000);
                #endif /* BOOT_SLOW */
                LoadEEPROM_OS(i);
                break;
            default:
                break;
        }
    }
#endif /* MOD_FlashFS */
    
    // Init Keyboard/Input
#ifdef MOD_Input_KB_PS2
    InitKB_PS2();
#endif
    
    #ifdef BOOT_SLOW
    OS_delay_ns(5000000);
    #endif /* BOOT_SLOW */

    d_clr();
    // do application specific inits here
    
    while(1)
    {
        // Do OS own stuff here
        OS_Stuff();
        
        // Now come the runlevels
        switch(OS.runlevel)
        {
            case RL_Boot:           // <editor-fold defaultstate="collapsed" desc="RL_Boot">   
                // After Booting the OS, switch to runlevel defined in DevelOS.h
                OS_SetRunlevel(Startmode);
                break;//</editor-fold>
            case RL_Standby:        // <editor-fold defaultstate="collapsed" desc="RL_Stanby">
                DoStandby();
                break;//</editor-fold>
            default:
                break;
        }
    }
}
