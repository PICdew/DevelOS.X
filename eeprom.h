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


unsigned char EE_check(void);
void EE_format(void);
unsigned char EE_read_byte(unsigned int eeprom_address);
unsigned char EE_read_block(unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len);
unsigned char EE_write_block (unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len);
//unsigned char EE_check_block(unsigned int eeprom_start_address, unsigned char len);
unsigned char LoadEEPROM_OS(unsigned char block);
unsigned char SaveEEPROM_OS(unsigned char block);
void InitEEPROM(void);

#endif /* MOD_FlashFS */

#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_H */

