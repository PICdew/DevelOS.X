/*      DevelOS
 *
 * File:    ps2-keyboard.h
 * Module:  MOD_Input_KB_PS2
 * Author:  Steffis PC
 * Destiny: PS2-Keyboard Driver
 *
 * Created on 27. April 2013, 21:16
 */

#ifndef PS2_KEYBOARD_H
#define	PS2_KEYBOARD_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "./DevelOS.h"

#ifdef MOD_Input_KB_PS2

    extern volatile struct KB_PS2 {
        volatile unsigned char RxBuff[4];
        volatile unsigned char TxBuff[2];
        volatile unsigned int BitBuffer;
        volatile unsigned char BitCounter;
        volatile unsigned char ByteCounter;
        volatile char isExtended;
        volatile char isBreak;
        char Shift;
        char Strg;
        char Alt;
    } Keyboard;

    void InitKB_PS2(void);
    unsigned int getKeyCode(unsigned int ScanCode);
    void PS2_Byte(unsigned int byte);
    extern const rom char PS2_Codes[3][104];                // mapping of the keys --> ascii-codes. this defines the keyboard layout

#endif




#ifdef	__cplusplus
}
#endif

#endif	/* PS2_KEYBOARD_H */

