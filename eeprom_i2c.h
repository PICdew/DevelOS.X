/* 
 * File:   eeprom_i2c.h
 * Author: Steffi
 *
 * Created on 19. April 2016, 22:33
 */

#ifndef EEPROM_I2C_H
#define	EEPROM_I2C_H
    
#ifdef	__cplusplus
extern "C" {
#endif
    
#include "./DevelOS.h"

#define I2C_Chips 2

    // Adressing Modes
#define I2C_3Ext_8Int       0x01            // use 3 lower bits of device address for bank switching, 8 bit internal address (e.g. ST24C08)
#define I2C_0Ext_16Int      0x02            // no external address bits used, but 16 bit internal address (e.g. AT24C32)
 
#pragma udata I2C_Data
extern struct I2Ceeprom {
        unsigned int        Daddr;          // device bus base address
        unsigned long       Delay;          // timing parameter for this device
        unsigned char       AddressMode;    // how to adress this device
        unsigned char       MBWsize;        // how many bytes can be written sequencially
    } i2c_chip;
#pragma udata
    
    char i2c_chip_init(unsigned int     Daddr,          // device bus base address
                        unsigned long   Delay,          // timing parameter for this device
                        unsigned char   AddressMode,    // how to adress this device
                        unsigned char   MBWsize         // how many bytes can be written sequencially
    );
    
    unsigned int I2C_eeReadByte(struct I2Ceeprom device, unsigned int b_adr);
    unsigned char I2C_eeWriteByte(struct I2Ceeprom device, unsigned int b_adr, unsigned char byte);
    char I2C_eeReadBlock(struct I2Ceeprom device, unsigned long block, unsigned char *ram_start_address);
    unsigned char I2C_eeWriteBlock(struct I2Ceeprom device, unsigned long block, unsigned char *ram_start_address);
    unsigned char I2C_eeCheckBlock(struct I2Ceeprom device, unsigned long block);

// Addressing Modes
       //<editor-fold defaultstate="collapsed" desc="List of Supported Devices">
/* Atmel 2-Wire Serial EEPROM
 *  AT24C32 : 32K (4096 x 8)
 *  AT24C64 : 64K (8192 x 8) 
 *  Clocks  : 100 kHz, 400 kHz @ 5V
 *  Adress  : 16 bit 
 *  W Modes : Page Write Mode (32 Bytes)
 *  R Modes : Single Byte, Sequential
 *  Notes   :
 */

/* ST 8 Kbit Serial I2C Bus EEPROM
 *  with User-Defined Block Write Protection
 *  ST24C08, ST25C08, ST24W08, ST25W08 
 *  Clocks  : 100 kHz
 *  Adress  : 8 bit
 *  W Modes : Multibyte Write Mode (8/16 Bytes), Page Write Mode (16 Bytes)
 *  R Modes : Single Byte, Sequential
 *  Notes   : This device uses 3 address bits for banke-switching,
 *            so it appears as 8 devices on the Bus!
 */    
    // </editor-fold>
    
    // <editor-fold defaultstate="collapsed" desc="Device Definition Block">
    /* Block 0
     * Device Data
     * Byte 0   : 32bit : Delay Value of this Device
     *                      tells us how fast the device can go
     * Byte 4   : 32bit : number of 64 byte blocks on the device
     *                      up to 255 Gigabytes per Device (whoa!)
     * Byte 8   : 32bit : number of used Blocks 
     * Byte 12  : 32bit : start of CRC-Block
     *                      if zero   : none
     *                      else      : store 16 bit Checksum for each Block on the device
     *                                  takes one CRC-Block for 32 Data Blocks
     * Byte 16  : 8bit  : Maximum transfer size for multibyte write
     * Byte 17  :
     * Byte 61  : 16bit : CRC for Block 0
     * Byte 63  : 8bit  : FlashFS Signature Byte
     */ 
#define I2C_DDB_delay   0
#define I2C_DDB_blocks  4
#define I2C_DDB_crc_blk 12
#define I2C_DDB_mbw     16
#define I2C_DDB_crc     61
#define I2C_DDB_sig     63
    
    //</editor-fold>
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_I2C_H */

