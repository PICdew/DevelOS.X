/* 
 * File:   eeprom.h
 * Author: Steffis PC
 *
 * Created on 20. April 2013, 21:29
 */

#ifndef EEPROM_H
#define	EEPROM_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef MOD_FlashFS

void EE_format(void);                                   // format the internal eeprom
void InitEEPROM(void);                                  // set all the addresses and read signature of each block TODO: also check crc for each block
char EE_load_block(unsigned char block);                // read one block from eeprom into the buffer. also checks crc
char EE_save_block(unsigned char block);                // calculate crc and save block to eeprom
unsigned char LoadEEPROM_OS(unsigned char block);       // Loads data from eeprom directly into OS
unsigned char SaveEEPROM_OS(unsigned char block);       // Writes OS data to eeprom block

// <editor-fold defaultstate="collapsed" desc="Internal helper functions. Should not be called directly">

unsigned char EE_check(void);
unsigned char EE_read_byte(unsigned int eeprom_address);    
unsigned char EE_read_block(unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len);
unsigned char EE_write_block (unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len);
//unsigned char EE_check_block(unsigned int eeprom_start_address, unsigned char len);

//</editor-fold>

#endif /* MOD_FlashFS */

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

