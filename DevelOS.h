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

#include "./runlevel.h"
#include "./events.h"
#include "./hardware.h"

#define REF 8
#define COUNT 9
#define REFval 10

#define BOOT_SLOW                           // This will add some delays to the startup process
#define Startmode           RL_Standby      // OS shall switch to this after booting
#define ResetToken          0xAA            // this is for resetting the os in debug
#define EventBuffer         32              // Length of the internal Event Queue
#define ISR_LF_Count        2               // Number of LF-Counters for the ISR-Routine. 
                                            // These are like programmable software-timers, running at (Fosc / 4) /64k
#define ISR_HF_Count        2               // Number of HF-Counters for the ISR-Routine. 
                                            // These are like programmable software-timers, running at (Fosc / 4) /256
/* Remember: The high priority ISR will go through a loop for the HF and LF Count. So it is important to keep these numbers as low as possible */
#define DefaultBrightness   7

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
    volatile unsigned int                Count;
    volatile unsigned int                Wait;
} isr_lf_count[ISR_LF_Count], isr_hf_count[ISR_HF_Count];
#pragma udata

/*
#pragma romdata system_strings
extern const rom char modestring[8][11];
extern const rom char setupstring[5][6][11];
extern const rom char PWMstring[6][11];
#pragma romdata
*/

// Functions in DevelOS.c
void InitOS(void);
void OS_Stuff(void);
void OS_delay_ns(unsigned long nanoseconds);
unsigned long getCPUClock(void);
unsigned long reflect (unsigned long crc, int bitnum) ;
unsigned long crc32(unsigned char* p, unsigned long len);
void InitChip(void);
void InitGlobals(void);
void Wait1S(void);
void ConvertVoltages(void);
void HandleReset(void);
unsigned char addEvent(const unsigned char type, const unsigned int data);
void delEvent(void);
void ScanADC(void);
void OS_SetRunlevel(unsigned char runlevel);
int crc16(char* ptr, char len);


#ifdef	__cplusplus
}
#endif

#endif	/* DEVELOS_H */

