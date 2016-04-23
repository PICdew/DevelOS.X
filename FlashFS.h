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
    #define EE_bytes_System     21      // 21 bytes used for now, 15 system, 6 rtc TODO: assign dynamically

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
                                        // Block 2+3 : Keypad Calibration Data (for MOD_Input_keypad )
    } Flash; // </editor-fold>
    
    void InitFlash(void);

//    <editor-fold defaultstate="collapsed" desc="#defines for easy access to Data Block">
        // Byte 0   : How many Blocks does this device have internal?
        // Byte 1   : How many external devices do we have?
        // Byte 2   : external adress for first external device
        //            for i2c devices, the first 4 bits are fixed.
        //            so we will use them as identifier for the type of device
        //            0b1001 = spi eeprom
        //            0b1010 = i2c eeprom with 8bit internal adressing
        //            0b1011 = i2c eeprom with 16bit internal adressing
        //            This allows up to 16 diffrent drivers to handle up to 16 devices each
        // Byte 3   : external adress for second external device
        // ......
        // Byte 58  : external adress for last external device
        //            this allows up to 57 external devices
        // Byte 59  : 32 bit CRC checksum for FlashFS Block
#define FFS_int_blocks      0
#define FFS_ext_devices     1
#define FFS_Device_1        2
#define FFS_Device_2        3
#define FFS_Device_3        4
// if you have more devices, continue here.
#define FFS_Data_CRC        61   
#define FFS_signature       63

    // </editor-fold>
    
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* FLASHFS_H */

