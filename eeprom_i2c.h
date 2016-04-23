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
    
#define I2C_devID           0b10100000      // first 4 bits are the device code
    // Adressing Modes
#define I2C_3Ext_8Int       0x01            // use 3 lower bits of device address for bank switching, 8 bit internal address (e.g. ST24C08)
#define I2C_0Ext_16Int      0x02            // no external address bits used, but 16 bit internal address (e.g. AT24C32)
    
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
     * Byte 16  : 16bit : Maximum transfer size for multibyte write
     * Byte 18  :
     * Byte 61  : 16bit : CRC for Block 0
     * Byte 63  : 8bit  : FlashFS Signature Byte
     */ //</editor-fold>
    
    extern struct I2Ceeprom {
        unsigned char       Daddr;          // device bus base address
        unsigned long       Delay;          // timing parameter for this device
        unsigned char       AddressMode;    // how to adress this device
        unsigned int        MBWsize;        // how many bytes can be written sequencially
    } I2C_eprom;
    
    unsigned int ReadByte(unsigned char d_adr, unsigned int b_adr);
    unsigned char WriteByte(unsigned char d_adr, unsigned int b_adr, unsigned char byte);
    unsigned char ReadBlock(unsigned char d_adr, unsigned long block, unsigned char *ram_start_address);
    unsigned char WriteBlock(unsigned char d_adr, unsigned long block, unsigned char *ram_start_address);
    unsigned char CheckBlock(unsigned char d_adr, unsigned long block);

// Addressing Modes
    
#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_I2C_H */

