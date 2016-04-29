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
#define Slowboot            5000            // how slow shall i boot?
#define Startmode           RL_Standby      // OS shall switch to this after booting
#define Display_Freq        2               
#define ADC_Freq            1
#define PWM_Freq            1
#define ResetToken          0xAA            // this is for resetting the os in debug
#define EventBuffer         8              // Length of the internal Event Queue
#define LF_Count            3               // Number of LF-Counters 
                                            // These are like programmable software-timers, running at 32 Hz
#define ISR_HF_Count        5               // Number of HF-Counters for the ISR-Routine. 
                                            // These are like programmable software-timers, running at (Fosc / 4) /256
/* Remember: The HF counters will be activated inside the OS by incrementing OS.HFCounters 
 *           The number here just sets the maximum available HF Counters. 
 *           Each takes 5 bytes of RAM.
 *  */

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

        // PWM
    unsigned int                pwm0;
    unsigned int                pwm1;
    
        // Operational Conditions
    unsigned int                U3V3;       //TODO: verallgemeinern. diese gehören ins ADC-Modul
    unsigned int                U5V0;
    unsigned int                U12V;
    unsigned int                Temp;

        // System Timings
    unsigned long               CPUClock;      // CPU Clock in kHz
    unsigned long               Tinst;        // instruction time in nanoseconds
    volatile unsigned char      RTC_preloadH;
    volatile unsigned char      RTC_preloadL;    
    unsigned char               F_Display;
    unsigned char               F_ADC;
    unsigned char               F_PWM;
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
    signed char                 Channel;
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
#define LFT_pwm         3

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
unsigned char intClock(void);

    // Other Functions
unsigned long detectModules(void);
void ConvertVoltages(void);
void ScanADC(void);
void OS_SetRunlevel(unsigned char runlevel);
int crc16(char* ptr, char len);
void float2string(char * output, float value);

// <editor-fold defaultstate="collapsed" desc="#pragma romdata system_strings">

#define sysstrings          14      // how many do we have?

#define sysstr_void         0    //    "          \0",     // empty string with trailing zero
#define sysstr_develos      1    //    "DevelOS\0   ",     // develos 
#define sysstr_version      2    //    "8bit v0.1\0 ",     // develos version 
#define sysstr_display      3    //    "Display:\0  ",     // display
#define sysstr_flashfs      4    //    "FlashFS\0   ",     // flashfs
#define sysstr_formating    5    //    "formating\0 ",     // formating
#define sysstr_corrupt      6    //    "corrupted!\0",     // corrupt
#define sysstr_eeprom       7    //    "EEPROM :\0  ",     // eeprom
#define sysstr_used         8    //    "Used\0      ",     // used
#define sysstr_block        9    //    "Block \0    ",     // block
#define sysstr_error        10    //    "Error \0    "      // Error
#define sysstr_free         11    //    "Free\0      "      // Error
#define sysstr_lps          12    //    "LPS:\0      "      // lps
#define sysstr_idle         13    //    "% Idle\0    "      // % idle

#pragma romdata system_strings
extern const rom char sys_string[sysstrings][11];     // system strings need to be constant length. 10 bytes plus zero
#pragma romdata



//</editor-fold>

#ifdef	__cplusplus
}
#endif

#endif	/* DEVELOS_H */