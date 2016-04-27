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
    
    // UART
#define EV_uart_rx      0x30    // data = recieved byte
#define EV_uart_tx      0x31    // byte transmit completed
#define EV_uart_error   0x32    // data = error code
    
    // Software timers
#define EV_LF_Timer     0xF0    // data = timer-number
#define EV_HF_Timer     0xF1    // data = timer-number
#define EV_Error        0xFF    // data = errorcode

    // LF-Timer SubEvents
#define EV_LFT_count    0x01    // count the counters
#define EV_LFT_rtc      0x02    // increment rtc
#define EV_LFT_display  0x03    // redraw display
#define EV_LFT_pwm      0x04    // refresh the pwm registers

    // Error Codes
    // UART Error Codes
#define EV_E_uart_frame 0x01    // uart reciever frame error
#define EV_E_uart_of    0x02    // uart reciever overflow
#define EV_E_uart_iof   0x03    // internal buffer overflow
    
    // OS Error Codes
#define EV_E_RLinv      0x04    // invalid runlevel
#define EV_E_EVinv      0x05    // unhandled Event
    
    // I2C Error Codes
#define EV_i2cerr_of    0x06    // Overflow error
#define EV_i2cerr_wc    0x07    // write collission
    

    
#ifdef	__cplusplus
}
#endif

#endif	/* EVENTS_H */
