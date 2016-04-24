/* 
 * File:   FlashFS.h
 * Author: Steffis PC
 *
 * Created on 27. April 2013, 20:28
 */

#ifndef FLASHFS_H
#define	FLASHFS_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "./DevelOS.h"

#ifdef MOD_FlashFS

    #define EE_Blocksize        64
    #define EE_Blocks           4       // 256 Byte, these exist on all PICs
    //#define EE_bytes_System     21      // 21 bytes used for now, 15 system, 6 rtc TODO: assign dynamically

    // Block signatures
    #define EE_sig_FlashFS      0x01
    #define EE_sig_System       0x02
    #define EE_sig_Keypad       0x03
    #define EE_sig_Void         0xFF
    #define EE_sig_Free         0xAA

    // <editor-fold defaultstate="collapsed" desc="RAM Buffer for Flash Data">
extern struct Flash_Data {
    unsigned char Data[EE_Blocksize];   // store FlashFS Data Block here

    struct EEPROM {
        unsigned char UsedBlocks;       // internal blocks used
        unsigned char Blocks;           // internal blocks total
        struct {
            unsigned int adress;        // offset from eeprom start
            unsigned char signature;    // last byte of block
        } Block[EE_Blocks];
    } eprom;
                                        // Block 0   : FlashFS internal Data
                                        // Block 1   : DevelOS internal Data
                                        // Block 2+3 : Buffer for paging
    } Flash; // </editor-fold>
    
    char InitFlash(void);

//    <editor-fold defaultstate="collapsed" desc="#defines for easy access to Data Block">
        // Byte 0   : 8bit  : How many Blocks does this device have internal?
        // Byte 1   : 8bit  : How many external devices do we have?
        // Byte 2   : 8bit  : external adress for first external device
        // Byte 3   : 8bit  : external adress for second external device
        // ......
        // Byte 60  : 8bit  : external adress for last external device. This allows up to 58 external devices
        // Byte 61  : 16bit : CRC checksum for FlashFS Block
        // Byte 63  : 8bit  : FlashFS Signature Byte
#define FFS_int_blocks      0
#define FFS_ext_devices     1
#define FFS_Device0         2
#define FFS_Data_CRC        61   
#define FFS_signature       63

    // </editor-fold>
    
    // device type identifiers
#define I2C_EEPROM          0xA0
#define I2C_EEPROM_16       0xB0

#endif

#ifdef	__cplusplus
}
#endif

#endif	/* FLASHFS_H */

