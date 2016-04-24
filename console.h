/* 
 * File:   console.h
 * Author: Steffi
 *
 * Created on 24. April 2016, 16:23
 */

#ifndef CONSOLE_H
#define	CONSOLE_H

#include "DevelOS.h"

#ifdef	__cplusplus
extern "C" {
#endif

#pragma udata CON
extern struct CONSOLE {
    unsigned char command[CMD_Buffer];       // command buffer
    // TODO: Console Buffer instead of double diplay buffer
} console;    
#pragma udata
    
/* Constant Strings in Program Memory */

// <editor-fold defaultstate="collapsed" desc="Strings in ROM">
#pragma romdata
    // System Strings
/*static const rom char* sysstr[]={
    "-=< vinculum monitor fw 0.02 >=-\0",
    "READY\0",
    "OK\0",
    "ERROR\0",
    "RESTART\0",
    "PIC 18F 46K20 ->\0",
    "LCD EA  W204  ->\0",
    "EEPROM Config ->\0",
    "µC>\0"
};*/          

    // console commands
/*static const rom char* cmdlist[]={
    "at",           // check alive
    "rst",          // reset
    "adc ",         // sub commands for adc
    "help",         // help text
    "setbd ",       // set baudrate
    "lcd ",         // subcommands for lcd
    "rtc",          // subcommands for rtc
    ""              // Terminator
};*/                  


    // ADC strings
#ifdef MOD_ADC
/*static const rom char * adcstr[]={
    "(3.3V RPi)\0",
    "( 5 V RPi)\0",
    "( 5 V ATX)\0",
    "(12 V ATX)\0"
};*/          
#endif /* MOD_ADC */

/*static const rom char * helptext[]={
    "\rCommands: \r\r\0",
    "  help    : This Message \r\0",
    "  at      : Return OK \r\0",
    "  rst     : Reset PIC \r\0",
    "  adc X   : Print AD-Channel X \r\0",
    "  adc r   : Restart ADC \r\0",
    "          : Scan Sequence \r\0",
    "  adc a   : Print all AD \r\0",
    "  setbd X : Set Baudrate \r\0",
    "  setbd p : Set Baud permanent \r\0",
    "  lcd x s : Print s to LCD \r\0",
    "          : Line X \r\0",
    "  rtc     : print RTC values\r\0",
    "  rtc s X : sync rtc & OSCTUNE\r\0"
    "\rCase insensitive, all other \r\0",
    "should return ERROR\r\0",
    " "
};*/        // Help text
// </editor-fold>

#ifdef	__cplusplus
}
#endif

#endif	/* CONSOLE_H */

