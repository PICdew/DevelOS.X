/*      DevelOS
 *
 * File:        DevelOS.c
 * Author:      Steffis PC
 * Desttiny:    basic OS functions
 *
 * Created on 21. April 2013, 00:08
 */
#include "./DevelOS.h"

        // Initialisation

// <editor-fold defaultstate="collapsed" desc="void OS_HandleReset(void)">
/****************************************************************\
 *                  DevelOS Reset Handler                       *
 *                                                              *
 * Read the relevant registers to find out what happened        *
 * We need a Reset-Token to see if we are debugging             *
 * There is a counter for each reset, so we can check what      *
 * goes wrong                                                   *
 * Ideally, this function should return some kind of error code *
 * that can be passed to InitOS(), where all other inits should *
 * take place                                                   *
 *                                                              *
 * TODO: act on the reset                                       *
 *       watchdog-reset should disable dangerous functions      *
\***************************************************************/

void OS_HandleReset(void)
{
    if(OS.Rdetect != ResetToken)  // Assume Power on Reset
    {
        InitOS();
    }
    else                    // WTF?
    {
        OS.Rcount++;
    }

    if(STKPTRbits.STKFUL)
    {
        OS.RcountSO++;
    }
    else if (STKPTRbits.STKUNF)
    {
        OS.RcountSU++;
    }

    if(RCONbits.RI)     // Reset came from Software
    {
        RCONbits.RI=0;
        OS.RcountRI++;
    }

    if(RCONbits.POR)    // Power-on-Reset
    {
        RCONbits.POR=0;
        OS.RcountPOR++;
    }

    if(RCONbits.BOR)    // Brown-out Reset
    {
        RCONbits.BOR=0;
        OS.RcountBOR++;
    }

    if(RCONbits.TO)     // Watchdog timeout
    {
        RCONbits.TO=0;
        OS.RcountWDT++;
    }
    //InitChip();
    //InitGlobals();
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_InitGlobals(void)">
/****************************************************\
 *            Initialize Global Variables           *
 *                                                  *
 * just some save values should be set here         *
 * meaningful values will be set later              *
\****************************************************/
void OS_InitGlobals(void)      // Initialize Global Variables
{
    int i,x,y;

    for(i=0;i<7;i++)
    {
        ADC_Data.channel[i]=0;
    }
    ADC_Data.vref=0;
    ADC_Data.count=15;          // Read internal reference first
    ADC_Data.refvalue=1203000;    // datasheet says 1,2V. this value is for finetuning (in µV)
    
    for (i=0;i<LF_Count;i++)
    {
        lf_count[i].count = 0;
        lf_count[i].wait = 0xFFFF;       
    }
    for (i=0;i<ISR_HF_Count;i++)
    {
        isr_hf_count[i].Count = 0;
        isr_hf_count[i].Wait = 0xFFFF;       
    }
    
#ifdef MOD_rtc
    rtc.days=0; // TODO: read from eeprom
    rtc.hour=0;
    rtc.mins=0;
    rtc.mont=0;
    rtc.secs=0;
    rtc.year=0;
#endif
    
#ifdef MOD_HardPWM
    OS.pwm0 = 0;
    OS.pwm1 = 0;
#endif
    
#if defined MOD_UART
    uart.rx_bytes = 0;
    uart.tx_bytes = 0;
    uart.baud = 1;
    uart.busy = 0;
    for(x=0;x<RX_BUFF_SIZE;x++)
    {
        uart.rx_buff[x] = 0;
    }
    for(x=0;x<TX_BUFF_SIZE;x++)
    {
        uart.tx_buff[x] = 0;
    }
#endif /* MOD_UART */
    
#ifdef MOD_Console
    c_clr();
#endif /* MOD_Console */
    
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_InitChip(void)">
/********************************************************************************\
 *                      Hardware Initialization                                 *
 *                                                                              *
 * Chip Hardware is initialized here. Configure Timers, ADCs and the like       *
 * Also set interrupt prioritys and enable interrupts                           *
\********************************************************************************/
void OS_InitChip(void)         // Configure the Hardware Modules
{
    // Init Clock
    
    if( intClock() )
    {
        // PIC configured for internal oscillator
        // so we configure it for max frequency here
        // TODO: this is processor specific, could write a function configureClock()
        OSCCONbits.IRCF=0b111;      // Internal Clock at Max (16 MHz)
        OSCCONbits.SCS1=1;          // 
        OSCTUNEbits.PLLEN=1;        // enable 4x PLL
        OSCTUNEbits.INTSRC=1;       // Use internal resonator
    }

    // Disable slew rate 
    SLRCONbits.SLRD=0;

    // AD-Wandler initialisieren
    #ifdef MOD_ADC
    ADCON0bits.CHS= 0xF;        // select first channel
    ADCON1bits.VCFG0 =0;        // Vref+ = VDD
    ADCON1bits.VCFG1 =0;        // Vref- = Vss
    ADCON2bits.ADFM = 1;        // 1=right-justified
    ADCON2bits.ACQT = 7;        // Aquisition Time (1=2TAD, 4= 8TAD, 7=20TAD)
    ADCON2bits.ADCS = 6;        // AD Clock  6= Fosc/64 (=1 MHz), 7=int_rc
    ADCON0bits.ADON =1;         // enable ADC
    //ADCON0bits.GO =1;         // Start ADC
    
    // Disable unused A/D-Chans
    ANSELHbits.ANS8=1;
    ANSELHbits.ANS9=1;
    ANSELHbits.ANS10=1;
    ANSELHbits.ANS11=1;
    ANSELHbits.ANS12=1;
    #endif
    
    // Timer0 as HF counter 
    T0CONbits.T08BIT = 1;       // 8-bit Mode
    T0CONbits.T0CS = 0;         // Use Fosc
    T0CONbits.T0PS = 0b010;     // Prescaler 1:8
    T0CONbits.PSA = 0;          // Use Prescaler
    T0CONbits.TMR0ON=0;         // Stop Timer0

    // Timer1 as LF counter, dedicated to rtc
    T1CONbits.RD16=1;
    T1CONbits.T1CKPS=3;         // prescale 8:1
    T1CONbits.T1OSCEN=0;        // don't use external resonator for now
    T1CONbits.T1RUN=0;          // do not use as clock source
    T1CONbits.NOT_T1SYNC=1;     // don't use external sync
    T1CONbits.TMR1CS=0;         // use cpu-clock as source
    T1CONbits.TMR1ON=0;         // Stop Timer1

    // external interrupt 0 for ps2-keyboard
    #ifdef MOD_Input_KB_PS2
        PS2_Clock_tr = 1;       // Always input
        PS2_Data_tr = 1;        // Input mostly
    #endif /* MOD_Input_KB_PS2 */

        
    // I2C Master Mode
    #ifdef MOD_FlashFS_extI2C
    SSPADD = 0x9C;              // set 100 kHz (400kHz->27h, 250kHz->3Fh)
    SSPSTATbits.SMP = 1;        // slew rate control, clear for 400 kHz
    SSPSTATbits.CKE = 0;        // disable SMBus
    SSPCON1bits.SSPEN = 1;      // enable I2C serial port
    SSPCON1bits.SSPM = 0b1000;  // I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))*/
    SCL = 1;                    // SCL input
    SDA = 1;                    // SDA input
    #endif /* MOD_FlashFS_extI2C */
    
    // PWM Module
    #ifdef MOD_HardPWM
    PWM0_t = 1;
    PWM0_t = 1;    
    PSTRCONbits.STRA = 1;   // disable all pins on the eccp-module
    PSTRCONbits.STRB = 0;
    PSTRCONbits.STRC = 0;
    PSTRCONbits.STRD = 0;
//    EPWM_p = 1;                 // then enable only the one the user wants
    PR2 = 0xFF;                 // this will give 10bit resolution down to 20 MHz with unaudible pwm frequency
    // set up CCP1
    CCP1CONbits.P1M = 0b00;     
    CCP1CONbits.CCP1M = 0b1100; // set standard pwm mode
    CCP1CONbits.DC1B = 0b00;    // the 2 lower bits of the 10 bit duty cycle
    CCPR1L = 0x00 ;             // the 8 higher bits of the 10 bit duty cycle
    // set up CCP2
    CCP2CONbits.CCP2M = 0b1100;
    CCP2CONbits.DC2B = 0b00;    // the 2 lower bits of the 10 bit duty cycle
    CCPR2L = 0x00 ;             // the 8 higher bits of the 10 bit duty cycle
    // configure timer 2 
    PIR1bits.TMR2IF = 0;        // reset the flag
    T2CONbits.T2CKPS = 0b00;    // no prescale. will run at about 60 kHz with sysclock 64MHz
    #endif /* MOD_HardPWM */

    // UART Module
    #ifdef MOD_UART
    BAUDCONbits.BRG16=1;        // set Baudrate Generator to 16bit mode
    TXSTAbits.BRGH=1;           // set Baudrate Multiplier
    SPBRGH=BAUD_H;              // set high byte of Baudrate
    SPBRG=BAUD_L;               // set low byte of Baudrate
    RX_t = 1;                   // set TRIS bits as input
    TX_t = 1;                   // set TRIS bits as input
    TXSTAbits.SYNC=0;           // set usart to async mode
    RCSTAbits.CREN=1;           // enable reciever circuit
    RCSTAbits.SPEN=1;           // enable the module
    TXSTAbits.TXEN=1;           // enable the transmitter. this will set the interrupt flag
    #endif /* MOD_UART */
    
    // configure irq priority
    RCONbits.IPEN = 1;          // enable priority
    INTCON2bits.TMR0IP = 1;     // Timer 0  : High Priority
    IPR1bits.TMR1IP = 1;        // Timer 1  : High Priority
    IPR1bits.TMR2IP = 0;        // Timer 2  : Low Priority
    IPR1bits.ADIP = 0;          // ADC      : Low Priority
    #ifdef MOD_UART
    IPR1bits.TXIP=0;            // UART tx  : Low Priority
    IPR1bits.RCIP=0;            // UART rx  : Low Priority
    #endif /* MOD_UART */

    // reset flags and enable irqs
    INTCONbits.TMR0IF = 0;      // Timer 0
    INTCONbits.TMR0IE = 1;
    PIR1bits.TMR1IF = 0;        // Timer 1
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR2IF = 0;        // Timer 2
    PIE1bits.TMR2IE = 1;

    #ifdef MOD_ADC              // ADC
    PIR1bits.ADIF = 0;          // clear AD interrupt
    PIE1bits.ADIE = 1;          // enable AD interrupt
    #endif /* MOD_ADC */
    
    #ifdef MOD_UART
    PIR1bits.TXIF=0;            // reset tx interrupt flag
    PIE1bits.TXIE=1;            // enable tx interrupt
    PIR1bits.RCIF=0;            // reset rx interrupt flag
    PIE1bits.RCIE=1;            // enable rx interrupt
    #endif /* MOD_UART */

    #ifdef MOD_Input_KB_PS2         // INT0 for PS2-Keyboard
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;
    #endif /* MOD_Input_KB_PS2 */

    #ifdef MOD_FlashFS_extI2C
    PIR1bits.SSPIF = 0;         // clear I2C interrupt
    PIE1bits.SSPIE = 1;         // enable I2C interrupt
    #endif /* MOD_FlashFS_extI2C */
    
    // finally, enable global irqs
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void InitOS(void)">
void InitOS(void)
{
    char i;

    OS.isInitialized=0;

    // Handle reset first
    OS.Rdetect=ResetToken;
    // TODO: evaluate these first!
    OS.Rcount=0;
    OS.RcountBOR=0;
    OS.RcountPOR=0;
    OS.RcountRI=0;
    OS.RcountWDT=0;
    OS.RcountSO=0;
    OS.RcountSU=0;
    
    // now, initialize runlevel
    OS.prev_runlevel=RL_Boot;
    OS.runlevel=RL_Boot;
    OS.runmode=0x00;
    
    // determine CPU clock and define System Timing
    
    OS.F_Display=Display_Freq;                 // TODO: create an array to store system timings
    OS.F_ADC=ADC_Freq;                     // use floats for timings to enable refresh rates < 1 / second
    OS.F_PWM=PWM_Freq;
    OS.RTC_preloadH=T1_preload_h;
    OS.RTC_preloadL=T1_preload_l;

    setTiming();

    // clear timing counters
    OS.FPS=0;
    OS.Framecounter=0;
    OS.Loopcount=0;
    OS.LPS=0;
    
    // clear the EventBuffer Pipeline
    for(i=0;i<EventBuffer;i++)
    {
        OS.Event[i].eventData=0;
        OS.Event[i].eventType=EV_Null;
    }
    OS.numEvents=0;

    // clear environmental variables
    OS.Temp=0;
    OS.U12V=0;
    OS.U3V3=0;
    OS.U5V0=0;

    // clear module hook variables
    OS.DisplayBrightness=DefaultBrightness;
    OS.DisplayInitialized=0;
    OS.DisplayType=0;
    
    // set the number of active HF Counters
    //OS.HFCounters=0;
    
    // OS should be in ready state now
    OS.isInitialized=1;

}// </editor-fold>

        // Event Handling

// <editor-fold defaultstate="collapsed" desc="unsigned char addEvent(const unsigned char type, const unsigned int data)">
unsigned char addEvent(const unsigned char type, const unsigned int data)
{
    if(OS.numEvents >= EventBuffer )
    {
        return 0;       // Buffer Full
    }
    OS.Event[OS.numEvents].eventType=type;
    OS.Event[OS.numEvents].eventData=data;
    OS.numEvents++;
    return 1;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void delEvent(void)">
void delEvent(void)
{
    /**********************************************************\
     * This Function rotates the Event Buffer.                *
     *                                                        *
     * TODO: Leave the Buffer static, and just have counters  *
     *          controlling where to read the current event,  *
     *          where to put new events and so on. this would *
     *          save lots of processor time                   *
    \**********************************************************/
    char i;
    if(OS.numEvents>0)       
    {
        for(i=0; i<EventBuffer-1; i++)
        {
            OS.Event[i].eventType=OS.Event[i+1].eventType;
            OS.Event[i].eventData=OS.Event[i+1].eventData;
        }
        OS.Event[EventBuffer-1].eventType=EV_Null;
        OS.Event[EventBuffer-1].eventData=0;
        OS.numEvents--;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_Event(void)">
void OS_Event(void)
{
    char temp;
    OS.Loopcount++;

    switch(OS.Event[0].eventType)
    {
        case EV_Null:       // <editor-fold defaultstate="collapsed" desc="EV_Null">
            OS.IdleCount++;
            delEvent();
            break;//</editor-fold>
            
        case EV_rtc:        // <editor-fold defaultstate="collapsed" desc="EV_rtc">            
            IncrementRTC();
            OS.FPS=OS.Framecounter;
            OS.LPS=OS.Loopcount;
            OS.CPUIdlePercent=  ( (float)( (float)OS.IdleCount / (float)OS.Loopcount ) * (float)100 );
            OS.Framecounter=0;
            OS.Loopcount=0;
            OS.IdleCount=0;
            delEvent();
            break;//</editor-fold>
            
        case EV_KeyMake:    // <editor-fold defaultstate="collapsed" desc="EV_KeyMake">
            #ifdef MOD_Input_KB_PS2
            temp=OS.Event[0].eventData;
            if( temp == 0x13)                   // Enter Key
            {
                // process command here
                d_cr();
            }
            else if( temp == 0x27)              // ESC Key
            {
                d_clr();
            }
            else if(isprint(temp))              // printable (ASCII) char
            {
                if(Display.cursor_x>19)         // watch for end of line
                {
                    d_cr();
                }
                Display.Buffer[Display.cursor_y][Display.cursor_x++]=temp;
            }
            delEvent();
            #else   /* MOD_Input_KB_PS2 */
            addEvent(EV_Error, EV_E_EVinv);
            delEvent();
            #endif /* MOD_Input_KB_PS2 */
            break;//</editor-fold>
            
        case EV_LF_Timer:   // <editor-fold defaultstate="collapsed" desc="EV_LF_Timer">
            
            switch(OS.Event[0].eventData)
            {
                case EV_LFT_count:
                    for(temp=0; temp<LF_Count;temp++)
                    {
                        lf_count[temp].count++;
                        if( lf_count[temp].count > lf_count[temp].wait)
                        {
                            addEvent(EV_LF_Timer, ( temp + EV_LFT_rtc));
                            lf_count[temp].count=0;
                        }
                    }
                    break;
                case EV_LFT_rtc:     // rtc
                    addEvent( EV_rtc, 0);
                    break;
                case EV_LFT_display:         // display
                    addEvent( EV_Display, 0);
                    break;
                case EV_LFT_pwm:
                    #ifdef MOD_HardPWM
                    CCP1CONbits.DC1B = OS.pwm0 & 0x0003;
                    CCPR1L = OS.pwm0 >> 2;
                    CCP2CONbits.DC2B = OS.pwm1 & 0x0003;
                    CCPR2L = OS.pwm1 >> 2;
                    #else

                    #endif /* MOD_HardPWM */
                    break;
                default:    // HF-Timer ?
              //      delEvent();
                    break;
            }
            delEvent();
            break;//</editor-fold>
            
        case EV_HF_Timer:   // <editor-fold defaultstate="collapsed" desc="EV_HF_Timer">
            switch(OS.Event[0].eventData)
            {
//                case EV_HFT_rtc:     // rtc
//                    delEvent();
//                    //addEvent( EV_rtc, 0);
//                    break;
//                case EV_HFT_display:         // display
//                    delEvent();
//                    //addEvent( EV_Display, 0);
//                    break;
                case 2:     // HF-Timer 2
                    break;
                case 3:     // HF-Timer 3
                    break;
                default:    // HF-Timer ?
                    delEvent();
                    break;
            }
            
            break;//</editor-fold>
            
        case EV_ScanMake:   // <editor-fold defaultstate="collapsed" desc="EV_ScanMake">
            #ifdef MOD_Input_KB_PS2
            if( (OS.Event[0].eventData == 0x12) | (OS.Event[0].eventData == 0x59) )
            {
                Keyboard.Shift=1;
            }
            else if( (OS.Event[0].eventData == 0x14) )
            {
                Keyboard.Strg=1;
            }
            else
            {
                addEvent( EV_KeyMake, getKeyCode( OS.Event[0].eventData ));
            }
            delEvent();
            #else   /* MOD_Input_KB_PS2 */
            addEvent(EV_Error, EV_E_EVinv);
            delEvent();
            #endif /* MOD_Input_KB_PS2 */
            break;//</editor-fold>
            
        case EV_ScanBreak:  // <editor-fold defaultstate="collapsed" desc="EV_ScanBreak">
            #ifdef MOD_Input_KB_PS2
            if( (OS.Event[0].eventData == 0x12) | (OS.Event[0].eventData == 0x59) )
            {
                Keyboard.Shift=0;
            }
            else if( (OS.Event[0].eventData == 0x14) )
            {
                Keyboard.Strg=0;
            }
            else
            {
                addEvent( EV_KeyBreak, getKeyCode( OS.Event[0].eventData ));
            }
            delEvent();
            #else   /* MOD_Input_KB_PS2 */
            addEvent(EV_Error, EV_E_EVinv);
            delEvent();
            #endif /* MOD_Input_KB_PS2 */
            break;//</editor-fold>
            
        case EV_Display:    // <editor-fold defaultstate="collapsed" desc="EV_Display">
            if(OS.DisplayInitialized==1)
            {
                OS.Framecounter++;
                RefreshDisplay();
            }                                    
            delEvent();
            break;//</editor-fold>
            
        case EV_Error:      // <editor-fold defaultstate="collapsed" desc="EV_Error">
            // do something about it
            c_print("Error \0");
            c_value(OS.Event[0].eventData);
            c_print("\n\0");
            // TODO: we might want to send errors directly to uart or something
            /*while(OS.Event[0].eventType == EV_Error)
            {
                delEvent();
            }*/
            delEvent();
            break;//</editor-fold>
            
        case EV_uart_error: // <editor-fold defaultstate="collapsed" desc="EV_uart_error">
            // do something about it
            switch(OS.Event[0].eventData)
            {
                case EV_E_uart_frame:
                    // recieve frame error
                    // as i have read somewhere, 
                    // these can be used for clock tuning somehow
                    break;
                case EV_E_uart_of:
                    // buffer overflow
                    break;
                default:
                    // unknown error
                    break;
            }
            delEvent();
            break;//</editor-fold>
            
        case EV_uart_rx:    // <editor-fold defaultstate="collapsed" desc="EV_uart_rx">
            #ifdef MOD_UART
            // recieved byte
            if(OS.Event[0].eventData == '\r')
            {
                // command recieved
                // copy uart rx buffer to command buffer
                
                // clear buffer
                for(temp=0;temp<RX_BUFF_SIZE;temp++)
                {
                    uart.rx_buff[temp] = 0;
                }
                uart.rx_bytes=0;
            }
            else
            {
                if(uart.rx_bytes<RX_BUFF_SIZE)
                {
                    uart.rx_buff[uart.rx_bytes] = OS.Event[0].eventData;
                    uart.rx_bytes++;
                }
                else
                {
                    addEvent(EV_uart_error, EV_E_uart_iof);
                    uart.rx_bytes=0;
                    for(temp=0;temp<RX_BUFF_SIZE;temp++)
                    {
                        uart.rx_buff[temp] = 0;
                    }
                }
            }
            delEvent();
            #else   /* MOD_UART */
            addEvent(EV_Error, EV_E_EVinv);
            delEvent();
            #endif /* MOD_UART */
            break;//</editor-fold>
            
        case EV_uart_tx:    // <editor-fold defaultstate="collapsed" desc="EV_uart_tx">
            #ifdef MOD_UART
            // no more bytes to send
            // clear buffer
            //while(!TXSTAbits.TRMT); //wait for the last byte to finish
            //TXSTAbits.TXEN=0;
            clearBuffTX();
            uart.busy = 0;
            delEvent();
            #else   /* MOD_UART */
            addEvent(EV_Error, EV_E_EVinv);
            delEvent();
            #endif /* MOD_UART */
            break;//</editor-fold>
            
        default:            // <editor-fold defaultstate="collapsed" desc="Unhandled Event">
            // unknown message
            addEvent(EV_Error, EV_E_EVinv);
            delEvent();
            break;//</editor-fold>
    }
}// </editor-fold>

        // Delays

// <editor-fold defaultstate="collapsed" desc="void OS_delay_ns(unsigned long nanoseconds)">
/********************************************************\
 * Delay at least the specified number of nanoseconds   *
 *                                                      *
 * The overhead has to be finetuned for each processor  *
 * TODO: put the constants into #defines                *
\********************************************************/
void OS_delay_ns(unsigned int nanoseconds)
{
    //unsigned long loop;
    long temp,loop;
    
    // 26 * Tinst per loop, check, substract. find this with disassembly
    loop = 26 * OS.Tinst;    
    temp = nanoseconds ;
    
    // substract overhead for this preparation
    temp -= (95*OS.Tinst);
    
    while(temp>loop)
    {
        temp -= loop;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_delay_us(unsigned long microseconds)">
/********************************************************\
 * Delay at least the specified number of microseconds  *
 *                                                      *
 * The overhead has to be finetuned for each processor  *
 * TODO: put the constants into #defines                *
\********************************************************/
void OS_delay_us(unsigned int microseconds)
{
    long temp,loop;
    
    // 26 * Tinst per loop, check, substract. find this with disassembly
    loop = 26 * OS.Tinst;    
    temp = microseconds * 1000;
    
    // substract overhead for this preparation
    temp -= (108*OS.Tinst);
    
    while(temp > loop)
    {
        temp -= loop;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_delay_ms(unsigned long milliseconds)">
void OS_delay_ms(unsigned int milliseconds)
{
    unsigned int i;
    for(i=0; i<milliseconds; i++)
    {
        OS_delay_us(1000);
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_delay_1S(void)">
void OS_delay_1S(void)           // Do Nothing
{
    int i;
    for(i=0;i<Slowboot;i++)
    {
        OS_delay_us(10000);
    }
}// </editor-fold>

        // Timing Control

// <editor-fold defaultstate="collapsed" desc="void setTiming(void)">
void setTiming(void)
{
    unsigned long temp;
    float f_temp;
    
    OS.CPUClock=getCPUClock();
    temp = (OS.CPUClock ) / 4;      // instructions per millisecond
    temp = temp / 1000;             // instructions per microsecond
    f_temp= 1 / ((float) temp);     // microseconds per instruction
    OS.Tinst = 1000 * f_temp;       // nanoseconds per instruction //(1000000 / OS.CPUClock) * 4; // in ns
    
    // TODO: derive these from OS.CPUClock to enable clock switching
    if(intClock())
    {
        temp = OS.CPUClock / 8000;
    }
    else
    {
        temp = OS.CPUClock / 2000;
    }
    lf_count[LFT_rtc].wait = temp;      // postscale for rtc
    lf_count[LFT_display].wait = lf_count[LFT_rtc].wait / OS.F_Display;
    lf_count[LFT_adc].wait = lf_count[LFT_rtc].wait / OS.F_ADC;
    lf_count[LFT_pwm].wait = lf_count[LFT_rtc].wait / OS.F_PWM;
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned int getCPUClock(void)">
unsigned long getCPUClock(void)
{
    unsigned long temp=0;

    if( intClock() )
    {
        switch(OSCCONbits.IRCF)
        {
            case 0:
                temp = 31;
                break;
            case 1:
                temp=250;
                break;
            case 2:
                temp=500;
                break;
            case 3:
                temp=1000;
                break;
            case 4:
                temp=2000;
                break;
            case 5:
                temp=4000;
                break;
            case 6:
                temp=8000;
                break;
            case 7:
                temp=16000;
                break;
        }
        if(OSCTUNEbits.PLLEN==1)
        {
            temp=temp*4;
        }
    }
    else
    {
        temp = XTAL;
    }

    return temp;
}// </editor-fold>

        // Other Functions

// <editor-fold defaultstate="collapsed" desc="void ScanADC(void)">
void ScanADC(void)
{
    ADC_Data.count=0;
    ADCON0bits.CHS=ADC_Data.count;
    ADCON0bits.GO=1;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="int crc16(char* ptr, char len)">
int crc16(char* ptr, char len)
{
  char data, i;
  
  union 
  { 
    char  Byte[2]; 
    int Word; 
  } CRC16;
  
  CRC16.Word = 0xFFFF;
  
  for(i=0; i<=len; i++)
  {  
    data = ptr[i] ^ CRC16.Byte[0];
    data ^= data << 4;
    
    data * 8;
    CRC16.Byte[0] = CRC16.Byte[1] ^ (data >> 4) ^ PRODL;
    CRC16.Byte[1] = data ^ PRODH;
  }
  
  return CRC16.Word;  
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_SetRunlevel(unsigned char runlevel)">
void OS_SetRunlevel(unsigned char runlevel)
{
    switch(OS.runlevel)             // Do Exit stuff (cleanup, carry, ...)
    {
        case RL_ROM_Stat:       
            // <editor-fold defaultstate="collapsed" desc="RL_ROM_Stat">
            OS.runmode=60;
            OS.submode=0;
            // </editor-fold>
            break;
        case RL_TestKeypad:
            // <editor-fold defaultstate="collapsed" desc="RL_TestKeypad">
            OS.runmode=20;
            runlevel=RL_Setup;
            // </editor-fold>
            break;
        case RL_Boot:
            // <editor-fold defaultstate="collapsed" desc="RL_Boot">
            // done booting. first, clear the console
            c_clr();    // maybe the contents could be saved somewhere as boot log
            
            // Now start the Timers
            //T0CONbits.TMR0ON=1;       // Start Timer0
            T1CONbits.TMR1ON=1;         // Start Timer1
            #ifdef MOD_HardPWM
            T2CONbits.TMR2ON=1;         // start Timer 2
            #endif
            // </editor-fold>
            break;
        default:
            // invalid runlevel should cause an error message
            OS.runmode=0;
            OS.submode=0;
            break;
    }

    OS.prev_runlevel=OS.runlevel;   // prev_runlevel might be unnecessary
    OS.runlevel=runlevel;

    switch(OS.runlevel)             // Do Entry stuff (init, SFR's, ...)
    {
        case RL_Standby:
            //LCD.Light=0;
            break;
        default:
            //d_clr();
            break;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void ConvertVoltages(void)">
/****************************************************\
 *  Convert ADC-Values to actual Voltage Levels     *
 *                                                  *
 * TODO: take the voltage levels out of the OS      *
 *          and store them inside the ADC struct    *
 *          also, put this function in separate     *
 *          source file for the ADC module          *
 *          change the constants to #defines        *
\****************************************************/
void ConvertVoltages(void)  
{
    unsigned long temp,tmp1,tmp2,tmp3;

    tmp1=ADC_Data.refvalue;
    tmp2=ADC_Data.vref;
    tmp3=tmp1/tmp2;
    tmp1=ADC_Data.channel[0];
    tmp2=ADC_Data.channel[1];

    temp=tmp3 * 1024;                 // Calculate 3,3V
    OS.U3V3 = temp/1000;                                    // in mV

    temp= 2.01 * tmp3 * tmp1;      // Calculate 5,0V
    OS.U5V0=temp/1000;                                     // in mV

    temp= 5.89 * tmp3 * tmp2 ;       // Calculate 12,0V
    OS.U12V=temp/1000;                                      // in mV

    temp=0;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void float2string(char * output, float value)">
void float2string(char * output, float value)
{
    //char string[10];
    char temp[3]={0,0,0};
    //int count=0;
    int tmp=0;
    float max_p=100;
    float min_p=0.001;
    float p=max_p;
    char pre=0;

    if(value > (max_p * 10)-1 )
        return;

    while(p >= min_p)
    {
        tmp=0;
        temp[0]=0;
        temp[1]=0;
        temp[2]=0;

        while(value > p)
        {
            value -= p ;
            tmp++;
            pre=1;
        }
        if(tmp>0 | pre)
        {
            //itoa(temp, tmp, 16);
            strcat (output, temp);
        }

        p=p/10;
        if(p==1)
            pre=1;

        if(p==0.1)
            strcat (output, ".");
    }
}// </editor-fold>

unsigned char intClock(void)
{
    unsigned char temp;
    
    // Init Clock
    memcpypgm2ram(&temp, 0x300001, 1);
    temp = temp & 0x0F; // we need the last 4 bits of CONFIG1H
    
    if((temp == 0x08) || (temp == 0x09) )
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

// <editor-fold defaultstate="collapsed" desc="#pragma romdata system_strings">

#pragma romdata system_strings
const rom char sys_string[sysstrings][11]={
    "          \0",     // empty string with trailing zero
    "DevelOS\0   ",     // develos 
    "8bit v0.1\0 ",     // develos version 
    "Display:\0  ",     // display
    "FlashFS\0   ",     // flashfs
    "formating\0 ",     // formating
    "corrupted!\0",     // corrupt
    "EEPROM :\0  ",     // eeprom
    "Used\0      ",     // used
    "Block\0     ",     // block
    "Error\0     ",     // Error
    "Free\0      ",     // Free
    "LPS:\0      ",     // LPS
    "% Idle\0    "      // % idle
};
#pragma romdata


// </editor-fold>
