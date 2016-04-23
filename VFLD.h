/*      DevelOS
 * File:   VFLD.h
 * Author: Steffis PC
 * Destiny: VFLD Display Driver Header
 *
 * Created on 31. März 2013, 20:43
 */

#ifndef _VFLD_H_
#define _VFLD_H_

#ifdef MOD_Display_VFLD
// Data Structure Interface
#pragma udata VFLD
extern struct VF_Display {
    char String[11];                // Text Buffer
    unsigned char Buffer[12][2];    // RAM Buffer       
            /* a buffer for VFL Display
             * do not confuse this one with 
             * struct Disp {
             *      unsigned char Buffer[BufferLines][20];
             *              ...
             *  } Display;
             *  in "display.h".
             * this one is the hardware buffer. 
             * If neccessary, this is the place to translate 
             * characters from ascii to whatever the display speaks.
             * this one was written for a display that i harvested from a
             * broken DVD-Player. It might work for other compatible SPI-Driven 
             * driven Displays, but you will most likely have to find out 
             * the bitmaps for your display manually. 
             * But this one might be already incompatible to display.h
             */
                     
    unsigned char Symbol[22];       // Symbol Flags
    unsigned char Duty;

    char Blink[11];
    unsigned char BlinkCount;
    unsigned char BlinkWait;

    char ScrollString[11];          // Scroll Text Buffer
    signed char ScrollSpeed;        // Scroll Direction, Speed and Flag
    unsigned char ScrollCount;      // internal scroll counter
    unsigned char ScrollWait;       // internal scroll counter
} VFLD;
#pragma udata

// Public Functions
void VFLD_Init(void);

void VFLD_Value(unsigned int value);
void VFLD_Time(char hour, char mins, char secs);
void VFLD_print(const unsigned char* string[]);
void VFLD_scroll(const unsigned char* string[], signed char speed);
void VFLD_clr(void);

void VFLD_Refresh(void);

// Internal Functions
void VFLD_WriteByte(int byte, int adress);
void VFLD_Write(int byte);
char VFLD_WriteBuffer(char mode);                       // for each display, a separate mode should be implemented.

#pragma romdata VFLD_ROM
extern const rom unsigned char VFLD_Bitmaps[48][2];       // contains Bitmaps for the 14-Segment-Display (2 Bytes / Charakter)
extern const rom unsigned char VFLD_Command[4];
#pragma romdata


// #defines for the Display

// All the symbolz (almost)
#define VFLD_RO     0  // - Segment 0, Low-Byte, Bit 3 : Ring
#define VFLD_RR     1  // - Segment 0, Low-Byte, Bit 4 : *r* Innen
#define VFLD_RY     2  // - Segment 0, Low-Byte, Bit 5 : *g* Innen
#define VFLD_RB     3  // - Segment 0, Low-Byte, Bit 6 : *b* Innen
#define VFLD_REC    4  // - Segment 1, Low-Byte, Bit 0 : *r* REC
#define VFLD_PROG   5  // - Segment 2, Low-Byte, Bit 0 : PROG
#define VFLD_RAND   6  // - Segment 2, Low-Byte, Bit 1 : RAND
#define VFLD_DEC    7  // - Segment 3, Low-Byte, Bit 0 : Komma
#define VFLD_CN1    8  // - Segment 3, Low-Byte, Bit 1 : Doppelpunkt
#define VFLD_ANGL   9  // - Segment 4, Low-Byte, Bit 0 : ANGLE
#define VFLD_ZOOM   10  // - Segment 4, Low-Byte, Bit 1 : ZOOM
#define VFLD_PBC    11  // - Segment 5, Low-Byte, Bit 0 : PBC
#define VFLD_CN2    12  // - Segment 5, Low-Byte, Bit 1 : Doppelpunkt
#define VFLD_STOP   13  // - Segment 0, Low-Byte, Bit 7 : Stop
#define VFLD_PAUS   14  // - Segment 0, High-Byte, Bit 0 : Pause
#define VFLD_PLAY   15  // - Segment 0, High-Byte, Bit 1 : Play
#define VFLD_R      16  // - Segment 0, High-Byte, Bit 2 : /R
#define VFLD_L      17  // - Segment 0, High-Byte, Bit 3 : L
#define VFLD_PCM    18  // - Segment 0, High-Byte, Bit 4 : PCM
#define VFLD_DTS    19  // - Segment 0, High-Byte, Bit 5 : *r* dts
#define VFLD_MIC    20  // - Segment 0, High-Byte, Bit 6 : *r* MIC
#define VFLD_DD     21  // - Segment 0, High-Byte, Bit 7 : Dolby Digital

#endif /* MOD_Display_VFLD */

#endif /* _VFLD_H_ */