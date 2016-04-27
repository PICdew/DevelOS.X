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

/*
 * Now, we declare the global Variables:
 */

// <editor-fold defaultstate="collapsed" desc="Global Variable Definitions">
#pragma udata OS_Data
struct OS_State OS;
#pragma udata AD_Data
volatile struct ADC_State ADC_Data;
#pragma udata ISR_Count
volatile struct Counter isr_hf_count[ISR_HF_Count];
#pragma udata LF_COUNT
struct LF_Counter lf_count[LF_Count];
#pragma udata

#if defined MOD_Display
//    #pragma udata TextBuffer
//    struct Disp Display;
//    #pragma udata HardBuffer

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
    #pragma udata
#endif /* MOD_FlashFS */

#ifdef MOD_I2C
    #pragma udata I2C_Data
    struct I2C_BUS i2c_bus;
    #pragma udata
#endif /* MOD_I2C */

#if defined MOD_FlashFS_extI2C
    #pragma udata I2C_Data
    struct I2Ceeprom  i2c_chip;
    #pragma udata
#endif /* MOD_FlashFS_extI2C */

#if defined MOD_I2C
    #pragma udata I2C_Data
    struct I2C_BUS i2c_bus;
    #pragma udata 
#endif /* MOD_I2C */

#if defined MOD_Input_KB_PS2
    volatile struct KB_PS2 Keyboard;
#endif /* MOD_Input_KB_PS2 */

#if defined MOD_UART
#pragma udata UART                          // uart data
volatile struct UART_DATA uart;
#pragma udata
#endif /* MOD_UART */

#if defined MOD_Console
#pragma udata CON
struct CONSOLE console;
#pragma udata
#endif /* MOD_Console */
// </editor-fold>

/*
 * Here come the service routines for IRQs:
 */
// <editor-fold defaultstate="collapsed" desc="Interrupt Service Routines">

        // <editor-fold defaultstate="collapsed" desc="High Priority">
#pragma code high_vector=0x08
void high_vector (void)
{
    _asm GOTO High_ISR _endasm
}
#pragma code
#pragma interrupt High_ISR nosave=section("ISR_Count"), section("UART")
void High_ISR(void)         
{
    char i;

    if(INTCONbits.TMR0IF)       // <editor-fold defaultstate="collapsed" desc="Timer 0 (HF Timer)">
    {
        INTCONbits.TMR0IF = 0;      //irq clear
        for(i=0; i<OS.HFCounters;i++)
        {
            if(isr_hf_count[i].Count++ > isr_hf_count[i].Wait)
            {
                addEvent(EV_HF_Timer, i);
                isr_hf_count[i].Count=0;
            }
        }
        INTCONbits.TMR0IE = 1;      //irq enable
    } // </editor-fold>

    else if(PIR1bits.TMR1IF)    // <editor-fold defaultstate="collapsed" desc="Timer 1 (LF Timer)">
    {
        PIR1bits.TMR1IF=0;          // clear interrupt flag
        T1CONbits.TMR1ON=0;         // Stop Timer1
        // send Event to OS
        addEvent(EV_LF_Timer, EV_LFT_count);
        // preload the timer
        TMR1H = T1_preload_h;
        TMR1L = T1_preload_l;
        T1CONbits.TMR1ON=1;         // Start Timer1
        PIE1bits.TMR1IE = 1;        // enable interrupt
    } // </editor-fold>
    
    else if(INTCONbits.INT0IF)  // <editor-fold defaultstate="collapsed" desc="Ext INT 0">
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
    
    else if(PIR1bits.RCIF)      // <editor-fold defaultstate="collapsed" desc="UART rx">
    {
                PIR1bits.RCIF=0;

        #ifdef MOD_UART

        if(RCSTAbits.FERR)
        {
            addEvent(EV_uart_error, EV_E_uart_frame);
            //tmp=RCREG;
        }else if(RCSTAbits.OERR)
        {
            addEvent(EV_uart_error, EV_E_uart_of);
            //tmp=RCREG;
        }else{
            //tmp=RCREG;
            addEvent(EV_uart_rx, RCREG);
            #ifdef UART_echo
            //TXREG=tmp;
            //TXSTAbits.TXEN=1;
            #endif /* UART_echo */
        }
        #endif /* MOD_UART */        

        //PIE1bits.RCIE=1;
    } // </editor-fold>
    
    else if(PIR1bits.TXIF)      // <editor-fold defaultstate="collapsed" desc="UART tx">
    {
        PIR1bits.TXIF=0;
#ifdef MOD_UART
        if(uart.tx_bytes>0)
        {
           uart.tx_bytes--;
           TXREG=uart.tx_buff[uart.tx_bytes];
        }
        else if(TXSTAbits.TRMT)
        {
                TXSTAbits.TXEN=0;
                //PIE1bits.TXIE=0;
                addEvent(EV_uart_tx, 0);
        }
//        else if(TXSTAbits.TRMT)
//        {
//            TXSTAbits.TXEN=0;
//        }
#endif /* MOD_UART */        
    } // </editor-fold>
    
    //INTCONbits.GIEH =1;
}
// </editor-fold>

        // <editor-fold defaultstate="collapsed" desc="Low Priority">
#pragma code low_vector=0x18
void low_vector (void)
{
    _asm GOTO Low_ISR _endasm
}
#pragma code
#pragma interruptlow Low_ISR nosave=section("UART")
void Low_ISR(void)     
{
    unsigned int temp;
    unsigned char tmp;
    
    if(PIR1bits.ADIF)       // <editor-fold defaultstate="collapsed" desc="ADC Module">
    {
        PIR1bits.ADIF=0;                        //  Clear IRQ
        
        temp=ADRESL;
        temp+=ADRESH*256;

        PIE1bits.ADIE=1;                        //  Enable IRQ
    } // </editor-fold>
    
    //INTCONbits.GIEL =1;
}
// </editor-fold>

// </editor-fold>

/*
 * Time to do something!
 */

#pragma code

void main(void)
{
    // temp variables for the main() loop must be defined here
    unsigned char i =0;
    signed char err =0;
    
    // First of all, look if this was an reset, and why
    OS_HandleReset();

    // Then, initialize global variables
    OS_InitGlobals();
    
    // Now, Init the Chip Hardware
    OS_InitChip();
    
    // Init OS to sane values
    InitOS();

    // do application specific inits here
    
    while(1)
    {
        // Do OS own stuff here
        OS_Event();
        
        // Now come the runlevels
        switch(OS.runlevel)
        {
            case RL_Boot:           
                // <editor-fold defaultstate="collapsed" desc="RL_Boot">  
                
                switch(OS.runmode)
                {
                    case RL_Boot_Display:       
                        // <editor-fold defaultstate="collapsed" desc="RL_Boot_Display"> 
                        sysprint(0, sysstr_develos, 1);
                        sysprint(0, sysstr_version, 0);
                        c_cr();
                        //Display Init
                        sysprint(0, sysstr_display, 1);
                        c_print(display_driver);
                        InitDisplay();
                        c_cr();
                        addEvent(EV_Display, 0);
                        OS.runmode++;
                        //</editor-fold>
                        break;
                    case RL_Boot_FlashFS:       
                        // <editor-fold defaultstate="collapsed" desc="RL_Boot_FlashFS">
                        #ifdef MOD_FlashFS
                            #ifdef BOOT_SLOW 
                            OS_delay_1S();
                            #endif
                            sysprint(0, sysstr_flashfs, 1);
                            err = InitFlash();
                            if(err != 0)
                            {
                                // no luck, handle error
                                if(err==-2)
                                {
                                    // no FlashFS Data Block found
                                    sysprint(0, sysstr_formating, 0);
                                    c_cr();
                                    EE_format();
                                }
                                else if(err==-1)
                                {
                                    // FlashFS Data Block invalid
                                    sysprint(0, sysstr_corrupt, 0);
                                    c_cr();
                                    EE_format();
                                }
                            }
                            else
                            {
                                c_pos(0, console.cursor_y);
                                sysprint(0, sysstr_eeprom, 1);
                                c_value( Flash.eprom.UsedBlocks );
                                c_print("/\0");
                                c_value( Flash.eprom.Blocks );
                                sysprint(1, sysstr_used, 0);
                                c_cr();
                                addEvent(EV_Display, 0);
                                OS.runmode++;
                                i=0;
                            }
                            //addEvent(EV_Display, 0);
                        #else /* MOD_FlashFS */
                        OS.runmode++;
                        #endif /* MOD_FlashFS */
                        //</editor-fold>
                        break;
                    case RL_Boot_Load:          
                        // <editor-fold defaultstate="collapsed" desc="RL_Boot_Load">
                        if(i >= EE_Blocks)
                        {
                            i=0;
                            OS.runmode++;
                        }
                        else
                        {
                            if(i>9)
                            {
                                sysprint(0, sysstr_block, 1);
                            }
                            else
                            {
                                sysprint(0, sysstr_block, 2);
                            }
                            c_value(i);
                            c_print(":\0");
                            #ifdef BOOT_SLOW
                            OS_delay_1S();
                            #endif /* BOOT_SLOW */
                            switch(Flash.eprom.Block[i].signature)
                            {
                                case EE_sig_FlashFS:                                    
                                    //c_print("FlashFS at \0");
                                    sysprint(1, sysstr_flashfs, 0);
                                    break;
                                case EE_sig_System:
                                    // This holds all the static settings for the OS
                                    sysprint(1, sysstr_develos, 0);
                                    LoadEEPROM_OS(i);
                                    break;
                                case EE_sig_Free:
                                    sysprint(1, sysstr_free, 0);
                                    break;
                                default:
                                    sysprint(1, sysstr_error, 0);
                                    break;
                            }
                            c_cr();
                            addEvent(EV_Display, 0);
                            i++;
                        }
                        //</editor-fold>
                        break;
                    case RL_Boot_Input:
                        // <editor-fold defaultstate="collapsed" desc="RL_Boot_Input">
                        // Init Keyboard/Input
                        #ifdef MOD_Input_KB_PS2
                        #ifdef BOOT_SLOW
                        OS_delay_ns(10000000);
                        #endif /* BOOT_SLOW */
                            InitKB_PS2();
                        #endif
                            //addEvent(EV_Display, 0);
                        OS.runmode++;
                        // </editor-fold>
                        break;
                    default:
                        // <editor-fold defaultstate="collapsed" desc="default">
                        if(OS.runmode >= RL_Boot_Done)
                        {
                            // After Booting the OS, switch to runlevel defined in DevelOS.h
                            #ifdef BOOT_SLOW
                            OS_delay_1S();
                            #endif /* BOOT_SLOW */
                            OS_SetRunlevel(Startmode);
                            //d_clr();
                            addEvent(EV_Display, 0);
                        }
                        else
                        {
                            OS.runmode++;
                        }
                        // </editor-fold>
                        break;
                }
                //</editor-fold>
                break;
            case RL_Standby:        
                // <editor-fold defaultstate="collapsed" desc="RL_Stanby">
                DoStandby();
                //</editor-fold>
                break;
            default:
                // <editor-fold defaultstate="collapsed" desc="Runlevel invalid">
                addEvent(EV_Error, EV_E_RLinv);     // invalid runlevel
                // </editor-fold>
                break;
        }
    }
}
