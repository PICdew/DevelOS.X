/* 
 * File:   i2c_bitbang.h
 * Author: Steffi
 *
 * Created on 23. April 2016, 09:50
 */

#ifndef I2C_BITBANG_H
#define	I2C_BITBANG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "DevelOS.h"
    extern struct I2C_BUS {
        unsigned char Buffer[I2C_Buffer];   // have a few bytes as buffer
        unsigned char BufPos;               // as a counter
        unsigned long Delay;                // for the timing of the bus
    } i2c_bus;
    
    char I2C_ByteIn(char ack);      // Read a byte from the bus, maybe ack it, return the byte
    char I2C_ByteOut(char byte);    // Write a byte to the bus, return the ack
    void I2C_Start(void);           // do the start condition
    void I2C_Stop(void);            // do the stop condition

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_BITBANG_H */

