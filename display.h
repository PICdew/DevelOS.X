/*      DevelOS
 *
 * File:    display.h
 * Author:  Steffis PC
 * Destiny: Generic Display Driver.
 *          be as hardware-independent as possible
 *
 * Created on 27. April 2013, 10:58
 *
 * Unterstützung für Textmodus
 * Puffer für 10 Zeilen a 20 Zeichen (vorläufig)
 * Viewport (Fenster) für kleinere Displays, eventuell auch für scroll-effekte
 *
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "DevelOS.h"

#ifdef MOD_Display

#define BufferLines     5

    extern struct Disp {
        unsigned char Buffer[BufferLines][20];  // Linebuffer für Text
        unsigned char Type[4];                  // TLA for Display-Type
        unsigned char cursor_x;                 //
        unsigned char cursor_y;                 // Cursor Position
        unsigned char view_x;                   //
        unsigned char view_y;                   // Viewport. Vorerst nur Zeile
        unsigned char light;                    // for backlight switching/dimming
    } Display;

    void InitDisplay(void);                     // Hardware Specific Initialisation
    void RefreshDisplay(void);                  /* Hardware Specific Refresh Routine
                                                 * determine visible area, translate for display, and transmit data
                                                 * HW-Specific stuff is all switched by defines 
                                                 */

    void d_clr(void);                                           // Clear Screen
    void d_pos(const unsigned char x, const unsigned char y);   // Set Cursor Pos
    void d_view(const unsigned char x, const unsigned char y);  // Set Viewport
    void d_print(const char* string[]);                         // print string to current cursor pos
    void d_cr(void);                                            // do carriage return. scroll linebuffer and set pos to 0,0
    void d_value(const unsigned int value);                     // convert integer to string and print to cursor. auto-omit zeroes

#endif /* MOD_Display */

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

