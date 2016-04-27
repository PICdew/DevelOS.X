/* 
 * File:   DevelOS.h
 * Author: Steffis PC
 *
 * Created on 21. April 2013, 00:08
 */

#ifndef DEVELOS_H
#define	DEVELOS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <delays.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <p18cxxx.h>

#include "./runlevel.h"
#include "./events.h"
#include "./hardware.h"

#define REF 8
#define COUNT 9
#define REFval 10

#define BOOT_SLOW                           // This will add some delays to the startup process
#define Slowboot            1000            // how slow shall i boot?
#define Startmode           RL_Standby      // OS shall switch to this after booting
#define ResetToken          0xAA            // this is for resetting the os in debug
#define EventBuffer         8              // Length of the internal Event Queue
#define LF_Count            3               // Number of LF-Counters 
                                            // These are like programmable software-timers, running at 32 Hz
#define ISR_HF_Count        5               // Number of HF-Counters for the ISR-Routine. 
                                            // These are like programmable software-timers, running at (Fosc / 4) /256
/* Remember: The high priority ISR will go through a loop for the HF Count. 
 * So it is important to keep that number as low as possible */

#pragma udata OS_Data    
extern struct OS_State {        
    // <editor-fold defaultstate="collapsed" desc="OS_State">

        // Internal States
    unsigned char               runlevel;
    unsigned int                runmode;
    unsigned int                submode;
    unsigned int                prev_runlevel;
    char                        isInitialized;

        // Event Handler
    volatile struct OS_Event {
        volatile unsigned char  eventType;
        volatile unsigned int   eventData;
    }                           Event[EventBuffer];
    volatile unsigned char      numEvents;

        // ISR Flags
    volatile char               HFCounters;
    /*volatile char               RefreshDisplay;
    volatile char               RefreshAD;
    volatile char               RefreshKP;
    volatile char               EventKP;
    volatile char               EventT0;
    volatile char               EventAD;
    volatile char               EventRTC;*/

        // PWM
    /*volatile unsigned int       HF_period0;
    volatile unsigned int       HF_period1;
    volatile unsigned int       HF_duty0;
    volatile unsigned int       HF_duty1;
    volatile char               HF_event0;
    volatile char               HF_event1;
    unsigned int                PWM_Value[4];*/

        // Operational Conditions
    unsigned int                U3V3;       //TODO: verallgemeinern. diese gehören ins ADC-Modul
    unsigned int                U5V0;
    unsigned int                U12V;
    unsigned int                Temp;

        // System Timings
    unsigned long               CPUClock;      // CPU Clock in kHz
    unsigned long               Tinst;        // instruction time in nanoseconds
    unsigned char               F_Display;
    unsigned char               F_ADC;
    volatile unsigned int       com_frame;
    unsigned long               Loopcount;
    unsigned int                LPS;
    unsigned int                IdleCount;
    unsigned char               CPUIdlePercent;

        // Display
    unsigned char               DisplayType;
    unsigned char               DisplayInitialized;
    unsigned char               DisplayBrightness;
    unsigned char               Framecounter;
    unsigned char               FPS;

        // Peripheral Conditions

        //Reset Counters
    unsigned char               Rdetect;
    unsigned int                Rcount;
    unsigned int                RcountPOR;
    unsigned int                RcountBOR;
    unsigned int                RcountWDT;
    unsigned int                RcountRI;
    unsigned int                RcountSU;
    unsigned int                RcountSO;
    // </editor-fold>
} OS;

#pragma udata AD_Data                       // TODO: AD-Funktionen auslagern
extern volatile struct ADC_State {
    volatile unsigned int       channel[7]; // only 7 of the 8 channels are available, 1 is for keypad
    volatile unsigned int       vref;       // store CH15(intRef) separate
    unsigned long               refvalue;   // the actual reference in µV
    volatile char               count;      // counter for the ISR
    volatile char               isValid;    // Data has been successfully accuired
    volatile char               isActual;   // Data has been recently updated
} ADC_Data;

#pragma udata ISR_Count
extern volatile struct Counter {
    volatile unsigned int       Count;
    volatile unsigned int       Wait;
} isr_hf_count[ISR_HF_Count];
#pragma udata

#pragma udata LF_COUNT
extern struct LF_Counter {
    unsigned int                count;
    unsigned int                wait;
} lf_count[LF_Count];
#pragma udata

#define LFT_rtc         0
#define LFT_display     1
#define LFT_adc         2

// Functions in DevelOS.c
    
    // Initialisation
void OS_HandleReset(void);
void OS_InitGlobals(void);
void OS_InitChip(void);
void InitOS(void);

    // Event Handling
unsigned char addEvent(const unsigned char type, const unsigned int data);
void delEvent(void);
void OS_Event(void);

    // Delays
void OS_delay_ns(unsigned int nanoseconds);
void OS_delay_us(unsigned int microseconds);
void OS_delay_ms(unsigned int milliseconds);
void OS_delay_1S(void);

    // Timing Control
unsigned long getCPUClock(void);
void setTiming(void);

    // Other Functions
unsigned long detectModules(void);
void ConvertVoltages(void);
void ScanADC(void);
void OS_SetRunlevel(unsigned char runlevel);
int crc16(char* ptr, char len);
void float2string(char * output, float value);

#pragma romdata system_strings
extern const rom char sys_string[14][11];     // system strings need to be constant length. 10 bytes plus zero
#pragma romdata

#define sysstr_void         0x00    //    "          \0",     // empty string with trailing zero
#define sysstr_develos      0x01    //    "DevelOS \0  ",     // develos 
#define sysstr_version      0x02    //    "8bit v0.1\0 ",     // develos version 
#define sysstr_display      0x03    //    "Display: \0 ",     // display
#define sysstr_flashfs      0x04    //    "FlashFS  \0 ",     // flashfs
#define sysstr_formating    0x05    //    "formating\0 ",     // formating
#define sysstr_corrupt      0x06    //    "corrupted!\0",     // corrupt
#define sysstr_eeprom       0x07    //    "EEPROM : \0 ",     // eeprom
#define sysstr_used         0x08    //    " Used\0     ",     // used
#define sysstr_block        0x09    //    "Block \0    ",     // block
#define sysstr_error        0x0A    //    "Error \0    "      // Error
#define sysstr_free         0x0B    //    "Free\0      "      // Error
#define sysstr_lps          0x0C    //    "LPS:\0      "      // lps
#define sysstr_idle         0x0D    //    "% Idle\0    "      // % idle
#ifdef	__cplusplus
}
#endif

#endif	/* DEVELOS_H */