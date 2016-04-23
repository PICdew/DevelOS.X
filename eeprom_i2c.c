#include "DevelOS.h"

#ifdef MOD_FlashFS_extI2C

unsigned int ReadByte(unsigned char d_adr, unsigned int b_adr)
{
    // start 
    
    // write d_adr, wait for ack
    // write b_adr, wait for ack
    // read byte, send ack
    // stop
    // return byte
}

unsigned char WriteByte(unsigned char d_adr, unsigned int b_adr, unsigned char byte)
{
    // start 
    
    // write d_adr, wait for ack
    // write b_adr, wait for ack
    // write byte, wait for ack
    // stop
    // return error code
}

unsigned char ReadBlock(unsigned char d_adr, unsigned long block, unsigned char *ram_start_address)
{
    char i;
    // determine start address of block
    // read 64 bytes sequential
    // write d_adr, wait for ack
    // write b_adr, wait for ack
    
    for(i=0; i<EE_Blocksize-1; i++)
    {
        // read byte, send ack
        // store into ram at given adress
    }
    // read last byte, send nack
    // stop
    // return error code
}

unsigned char WriteBlock(unsigned char d_adr, unsigned long block, unsigned char *ram_start_address)
{
    unsigned char loops;
    char i,j;
    char mbw;
    
    // determine start address of block
    // choose best write mode
    // get maximum multibyte size for device
    // loops = EE_Blocksize/mbw;
    
    // start
    // write d_adr, wait for ack
    // write b_adr, wait for ack
    
    // write 64 bytes to eeprom
    for(i=0; i<loops; i++)
    {
        for(j=0; j<mbw; j++)
        {
            // write byte, wait for ack
        }
    }
    // stop
    
    // return error code
}

unsigned char CheckBlock(unsigned char d_adr, unsigned long block)
{
    // determine address of CRC on device
    // determine start address of block
    // read CRC from device
    // read Block and calculate CRC
    // compare CRCs and return error code
}
#endif

