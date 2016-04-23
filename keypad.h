/* 
 * File:   keypad.h
 * Author: Steffis PC
 *
 * Created on 18. April 2013, 14:45
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

#ifdef	__cplusplus
extern "C" {
#endif


#define Scanlines 3         // Digital Output Pins
#define Keyrows 6           // A/D-Levels to use
#define Keynum 19           // Keys on Pad + 2 for Wheel
    
#define KP_0     0
#define KP_1     1
#define KP_2     2
#define KP_3     3
#define KP_4     4
#define KP_5     5 
#define KP_6     6
#define KP_7     7
#define KP_8     8
#define KP_9     9
#define KP_ST   10
#define KP_HT   11    
#define KP_L    12
#define KP_R    13
#define KP_C    16
#define KP_GN   14
#define KP_RT   15
#define WH_U    17
#define WH_D    18

// <editor-fold defaultstate="collapsed" desc="Keypad Data struct Deklaration">
#pragma udata KP_Data
extern struct Keypad {
    unsigned char Key[Keynum];      // 1 counter for each key
    
    volatile unsigned int Scanline[Scanlines];//Buffer for the AD-Values.  
    volatile char DataReady;
    
    struct {                        // This is the Layout and Calibration values.
        char line;
        unsigned int limit0;
        unsigned int limit1;
    } Matrix[Keynum];
} KP;
#pragma udata
// </editor-fold>

char ScanKeys(void);        // Take ADC-Value and Check for Keypress
char SetKeylimit(char code, char line, unsigned int value, char limit);
void InitKeypad(void);      // just set all to save values
void ScanKeypad(void);      // Start the IRQ-driven Scan Process
unsigned char LoadEEPROM_KP(unsigned char block);
unsigned char SaveEEPROM_KP(unsigned char block);


#ifdef	__cplusplus
}
#endif

#endif	/* KEYPAD_H */

