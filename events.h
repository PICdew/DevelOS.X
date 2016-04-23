/* 
 * File:   events.h
 * Author: Steffis PC
 *
 * Created on 28. April 2013, 20:08
 */

#ifndef EVENTS_H
#define	EVENTS_H

#ifdef	__cplusplus
extern "C" {
#endif

    // Main System Events
#define EV_Null         0x00    // data = void
#define EV_rtc          0x01    // data = void
#define EV_Display      0x02    // data = void

    // PS2 Keyboard
#define EV_KeyMake      0x10    // data = ASCII Code
#define EV_KeyBreak     0x11    // data = ASCII Code
#define EV_ScanMake     0x12    // data = PS2-Scancode
#define EV_ScanBreak    0x13    // data = PS2-Scancode

    // I2C
#define EV_I2C_rx       0x20    // data = recieved byte
#define EV_I2C_tx       0x21    // data = byte to transmit
#define EV_I2C_error    0x22    // data = error code
#define EV_I2C_reset    0x24    // data = void  (I2C Module needs reset)

    // Software timers
#define EV_LF_Timer     0xF0    // data = timer-number
#define EV_HF_Timer     0xF1    // data = timer-number
#define EV_Error        0xFF    // data = errorcode

    // HF-Timer SubEvents
#define EV_HFT_rtc      0x00
#define EV_HFT_display  0x01
    
    // I2C error codes
#define EV_i2cerr_of    0x00    // Overflow error
#define EV_i2cerr_wc    0x01    // write collission
    

    
#ifdef	__cplusplus
}
#endif

#endif	/* EVENTS_H */

