
#include "./DevelOS.h"

#ifdef MOD_FlashFS

void InitFlash(void)
{
    char i;
    unsigned int crc, crc_r;
    unsigned char tmp;
    unsigned int temp;
    signed char err;

    Flash.eprom.UsedBlocks=0;
    
    for(i=0;i<EE_Blocks;i++)
    {
        Flash.eprom.Block[i].adress=(i*EE_Blocksize) & 0x00FF;                                          // offset from eeprom start
        Flash.eprom.Block[i].signature=EE_read_byte(Flash.eprom.Block[i].adress + (EE_Blocksize-1) );   // last byte of block
        if(Flash.eprom.Block[i].signature != EE_sig_Free)
        {
            Flash.eprom.UsedBlocks++;
        }
        //Flash.eprom.Block[i].used=EE_read_byte(Flash.eprom.Block[i].adress + (EE_Blocksize-2) );        // bytes used, second-last byte of block
    }
    
    if(Flash.eprom.Block[0].signature != EE_sig_FlashFS)
    {
        EE_format();
        InitFlash();
    }
    else
    {
        // TODO: read FS-Block
        // read first Block
        EE_read_block(Flash.eprom.Block[0].adress, &Flash.Data, EE_Blocksize);
        
        // check the crc32
        crc     = crc16(&Flash.Data, EE_Blocksize-4);
        crc_r   = 0;
        crc_r   |= ((unsigned char) Flash.Data[FFS_Data_CRC]) << 8;
        crc_r   |= ((unsigned char) Flash.Data[FFS_Data_CRC+1]);

        if(crc == crc_r)
        {
            Flash.eprom.Blocks      = Flash.Data[FFS_int_blocks];
            /*for(i=0; i < Flash.Data[FFS_ext_devices] ; i++)
            {
                // Read info from each device
                //CloseI2C();
                //OpenI2C(MASTER, SLEW_OFF);

                temp    = EERandomRead( Flash.Data[FFS_Device_1+i], EE_Blocksize);
                tmp     = (char) temp & 0x00FF;
                err     = (signed char) ( temp >> 8 ) & 0x00FF;
                
                if( err <= 0)
                {
                    // Bus Error
                }
                else if(tmp != EE_sig_FlashFS)
                {
                    // new or invalid device
                    temp    = EEByteWrite( Flash.Data[FFS_Device_1+i], EE_Blocksize, EE_sig_FlashFS );
                    temp    = EERandomRead( Flash.Data[FFS_Device_1+i], EE_Blocksize);
                    tmp     = (char) temp & 0x00FF;
                    err     = (signed char) ( temp >> 8 ) & 0x00FF;
                    if( err <= 0)
                    {
                        // Bus Error
                    }
                    else if(tmp != EE_sig_FlashFS)
                    {
                        // invalid device
                    }
                    else
                    {
                        // new device, format
                    }
                }
                else
                {
                    // seems good, read data
                }
                //CloseI2C();
            }*/
        }
        else
        {
            // Block invalid, reformat
            EE_format();
            InitFlash();            
        }
        
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
        // Byte 63  : EE_sig_FlashFS
        d_cr();
        d_print("EEPROM: \0");
        d_value( Flash.eprom.Blocks - Flash.eprom.UsedBlocks);
        d_print("/\0");
        d_value( Flash.eprom.Blocks );
        d_print(" Free\n");
    }
}



#endif
