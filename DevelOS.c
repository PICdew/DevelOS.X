/*      DevelOS
 *
 * File:        DevelOS.c
 * Author:      Steffis PC
 * Desttiny:    basic OS functions
 *
 * Created on 21. April 2013, 00:08
 */
#include "./DevelOS.h"

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
    
    for (i=0;i<ISR_LF_Count;i++)
    {
        isr_lf_count[i].Count = 0;
        isr_lf_count[i].Wait = 0xFFFF;       
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
    
#ifdef MOD_Display
    for(y=0;y<BufferLines;y++)
    {
        for(x=0;x<20;x++)
        {
            Display.Buffer[y][x]=' ';
        }
    }
    Display.cursor_x=0;
    Display.cursor_y=0;
    Display.view_x=0;
    Display.view_y=0;
    Display.light=1;    // if light is only switched, it should be on by default. if dimmed, this should set minimum
#endif /* MOD_Display */
    
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
    OSCCONbits.IRCF=0b111;      // Internal Clock at Max (16 MHz)
    OSCCONbits.SCS1=1;          // 
    OSCTUNEbits.PLLEN=1;        // enable 4x PLL
    OSCTUNEbits.INTSRC=1;       // Use internal resonator
    
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
 /* ANSELHbits.ANS8=1;
    ANSELHbits.ANS9=1;
    ANSELHbits.ANS10=1;
    ANSELHbits.ANS11=1;
    ANSELHbits.ANS12=1;*/
    #endif
    
    // Timer0 as HF counter (rtc etc.)
    T0CONbits.T08BIT = 1;       // 8-bit Mode
    T0CONbits.T0CS = 0;         // Use Fosc
    T0CONbits.T0PS = 0b010;     // Prescaler 1:8
    T0CONbits.PSA = 0;          // Use Prescaler
    T0CONbits.TMR0ON=0;         // Stop Timer0

    // Timer1 as LF counter
    T1CONbits.T1CKPS=0;         // prescale 1:1
    T1CONbits.RD16=1;
    T1CONbits.T1OSCEN=0;
    T1CONbits.T1RUN=0;
    T1CONbits.NOT_T1SYNC=1;
    T1CONbits.TMR1CS=0;
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
    #endif /* MOD_FlashFS_extI2C 
    
    // PWM Module
    #ifdef MOD_HardPWM
    TRISCbits.TRISC2 = 1;
    PR2 = 0xFF ;
    CCPR1L = 0b01111100 ;
    CCP1CON = 0b00111100 ;
    T2CONbits.T2CKPS = 0b11;
    T2CONbits.TMR2ON = 1;
    #endif /* MOD_HardPWM */

    // configure irq priority
    RCONbits.IPEN = 1;          // enable priority
    INTCON2bits.TMR0IP = 1;     // Timer 0  : High Priority
    IPR1bits.TMR1IP = 1;        // Timer 1  : High Priority
    PIR1bits.TMR2IF = 0;        // Timer 2  : Low Priority
    IPR1bits.ADIP = 0;          // ADC      : Low Priority
    IPR1bits.SSPIP = 0;         // I2C      : Low Priority

    // reset flags and enable irqs
    INTCONbits.TMR0IF = 0;          // Timer 0
    INTCONbits.TMR0IE = 1;
    PIR1bits.TMR1IF = 0;            // Timer 2
    PIE1bits.TMR1IE = 1;

    #ifdef MOD_ADC              // ADC
    PIR1bits.ADIF = 0;          // clear AD interrupt
    PIE1bits.ADIE = 1;          // enable AD interrupt
    #endif /* MOD_ADC */

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
    OS.CPUClock=getCPUClock();
    OS.Tinst = (1000000 / OS.CPUClock) * 4; // in ns
    OS.F_Display=2;     // TODO: create an array to store system timings
    //OS.F_ADC=1;
    isr_hf_count[EV_HFT_rtc].Wait = ( 1000 * OS.CPUClock / 16 ) / 2048;
    isr_hf_count[EV_HFT_display].Wait = isr_hf_count[EV_HFT_rtc].Wait / OS.F_Display;

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
    
    // OS should be in ready state now
    OS.isInitialized=1;

    // Now start the Timers
    T0CONbits.TMR0ON=1;     // Start Timer0
    T1CONbits.TMR1ON=1;     // Start Timer1
}// </editor-fold>

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
    // just rotate the buffer by one, then set the last one zero
    char i;

    for(i=0;i<EventBuffer-1;i++)
    {
        OS.Event[i].eventType=OS.Event[i+1].eventType;
        OS.Event[i].eventData=OS.Event[i+1].eventData;
    }
    OS.Event[EventBuffer-1].eventType=EV_Null;
    OS.Event[EventBuffer-1].eventData=0;
    if(OS.numEvents>0)
        OS.numEvents--;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_Stuff(void)">
void OS_Stuff(void)
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
            break;//</editor-fold>
            
        case EV_LF_Timer:   // <editor-fold defaultstate="collapsed" desc="EV_LF_Timer">
            switch(OS.Event[0].eventData)
            {
                default:
                    delEvent();
                    break;
            }
            break;//</editor-fold>
            
        case EV_HF_Timer:   // <editor-fold defaultstate="collapsed" desc="EV_HF_Timer">
            switch(OS.Event[0].eventData)
            {
                case EV_HFT_rtc:     // rtc
                    delEvent();
                    addEvent( EV_rtc, 0);
                    break;
                case EV_HFT_display:         // display
                    delEvent();
                    addEvent( EV_Display, 0);
                    break;
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
#endif
            delEvent();
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
#endif
            delEvent();
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
            d_print("Error \0");
            d_value(OS.Event[0].eventData);
            d_print("\n\0");
            /*while(OS.Event[0].eventType == EV_Error)
            {
                delEvent();
            }*/
            break;//</editor-fold>
            
        default:
            // unknown message
            //addEvent(EV_Error, OS.Event[0].eventType);
            delEvent();
            break;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void ScanADC(void)">
void ScanADC(void)
{
    ADC_Data.count=0;
    ADCON0bits.CHS=ADC_Data.count;
    ADCON0bits.GO=1;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned long reflect (unsigned long crc, int bitnum)">
unsigned long reflect (unsigned long crc, int bitnum)
{

	// reflects the lower 'bitnum' bits of 'crc'

	unsigned long i, j=1, crcout=0;

	for (i=(unsigned long)1<<(bitnum-1); i; i>>=1) {
		if (crc & i) crcout|=j;
		j<<= 1;
	}
	return (crcout);
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
    switch(OS.runlevel)                                                         // Do Exit stuff (cleanup, carry, ...)
    {
        case RL_ROM_Stat:
            OS.runmode=60;
            OS.submode=0;
            break;
        case RL_TestKeypad:
            OS.runmode=20;
            runlevel=RL_Setup;
            break;
        default:
            OS.runmode=0;
            OS.submode=0;
            break;
    }

    OS.prev_runlevel=OS.runlevel;
    OS.runlevel=runlevel;

    switch(OS.runlevel)                                                         // Do Entry stuff (init, SFR's, ...)
    {
        default:
            d_clr();
            break;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void OS_delay_ns(unsigned long nanoseconds)">
void OS_delay_ns(unsigned long nanoseconds)
{
    unsigned long temp,loop;
    loop = 2 * OS.Tinst;    // 2* Tinst per loop, check, substract
    temp = nanoseconds ;
  //  temp -= (3*loop);
    
    while(temp>loop)
    {
        temp -= loop;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void ConvertVoltages(void)">
void ConvertVoltages(void)  // Convert ADC-Values to actual Voltage Levels
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
//    T0counters.RTCWait = (OS.CPUClock /4) /64;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned int getCPUClock(void)">
unsigned long getCPUClock(void)
{
    unsigned long temp=0;

#if defined(__18F46K20)
    // <editor-fold defaultstate="collapsed" desc="processor specific">
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
    }// </editor-fold>
#endif

    return temp;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="void Wait1S(void)">
void Wait1S(void)           // Do Nothing
{
   Delay10KTCYx(250);
   Delay10KTCYx(250);
   Delay10KTCYx(250);
   Delay10KTCYx(250);
   Delay10KTCYx(250);
   Delay10KTCYx(250);

}// </editor-fold>

/*
// <editor-fold defaultstate="collapsed" desc="#pragma romdata system_strings">
#pragma romdata system_strings
const rom char setupstring[5][6][11]={

{   "+ KEYPAD +",        // 0,0,[]   Main Menu
    "+ EEPROM +",
    "+  TAKT  +",
    "+ SYSTEM +",
    "  REBOOT  ",
    " ERLEDIGT "},

{   "SET MATRIX",        // 1,0,[]   Keypad Menu
    "TASTENTEST",
    "FINETUNING",
    " ERLEDIGT ",
    "          ",
    "          "},

{   "ROM STATUS",        // 2,0,[]   ROM Menu
    "FORMAT ROM",
    "ROM BACKUP",
    "WRITE   KP",
    "WRITE   OS",
    " ERLEDIGT "},

{   "ZEIT/DATUM",        // 3,0,[]   Clock Menu
    "CPU   TAKT",
    "OSC TUNING",
    "SYS TIMING",
    " ERLEDIGT ",
    "          "},

{   "OS FPS/LPS",        // 4,0,[]   System menu
    "OS  RESETS",
    "OS  UPTIME",
    "OS   ALARM",
    "REINIT  OS",
    " ERLEDIGT "},

};
const rom char modestring[8][11]={
    "SHOW   3,3\n",
    "SHOW   5,0\n",
    "SHOW  12,0\n",
    "HIGH   PWM\n",
    "LOW    PWM\n",
    "+ SET UP +\n",
    "SYSMONITOR\n",
    " SHUTDOWN \n"
};
const rom char PWMstring[6][11]={
    "CH1 D     \n",
    "CH1 P     \n",
    "CH2 D     \n",
    "CH2 P     \n",
    "+ PRESET +\n",
    "   EXIT   \n"
};
// </editor-fold>
*/