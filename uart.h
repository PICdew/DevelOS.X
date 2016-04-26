/* 
 * File:   uart.h
 * Author: Steffi
 *
 * Created on 24. April 2016, 16:02
 */

#ifndef UART_H
#define	UART_H

#include "./DevelOS.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
#ifdef MOD_UART    
    
#pragma udata UART                          // uart data
extern volatile struct UART_DATA {
    unsigned char rx_buff[RX_BUFF_SIZE];    // uart recieve buffer, =max cmd length
    volatile unsigned char tx_buff[TX_BUFF_SIZE];    // uart transmit buffer
    unsigned char rx_bytes;                 // rx counter
    volatile unsigned char tx_bytes;                 // tx counter
    unsigned char baud;                     // baudrate
    unsigned char busy;
    } uart;
#pragma udata

char setBaud(char cmd);
char sendString(const char string[],char len);
void clearBuffTX(void);

    // (53332) -> 300 baud @ 64 MHz
#define BAUD_H__300      0b11010000
#define BAUD_L__300      0b01010100
    // (13332) -> 1200 baud @ 64MHz
#define BAUD_H_1200     0b00110100
#define BAUD_L_1200     0b00010100
    // (6666) -> 2400 baud @ 64MHz
#define BAUD_H_2400     0b00011010
#define BAUD_L_2400     0b00001010
    // (1666) -> 9600 baud @ 64MHz
#define BAUD_H_9600     0b00000110
#define BAUD_L_9600     0b10000010
    // (138) -> 115.2k baud @ 64MHz
#define BAUD_H_115k     0b00000000
#define BAUD_L_115k     0b10001010

#pragma romdata 
static const rom char BAUD[8][3]={
    /* ID   BRH         BRL         String */
    { 0x00, 0b00000000, 0b00000000 },   // Invalid
    { 0x01, BAUD_H__300, BAUD_L__300 },   // 300 Baud
    { 0x02, BAUD_H_1200, BAUD_L_1200 },   // 1200 Baud
    { 0x03, BAUD_H_2400, BAUD_H_2400 },   // 2400 Baud
    { 0x04, BAUD_H_9600, BAUD_L_9600 },   // 9600 Baud
    { 0x05, 0b00000011 , 0b01000000  },   // 19.2k Baud
    { 0x06, 0b00000001 , 0b00010101  },   // 57.6k Baud
    { 0x07, BAUD_H_115k, BAUD_L_115k }    // 115.2k Baud
};

/*static const rom char* BDstring[]={
    "Invalid\0",
    "300\0",
    "1200\0",
    "2400\0",
    "9600\0",
    "19.2k\0",
    "57.6k\0",
    "115.2k\0"
};*/        // Baudrates
#pragma romdata

#endif /* MOD_UART */
#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

