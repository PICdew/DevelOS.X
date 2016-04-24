
#include "./DevelOS.h"

#ifdef MOD_FlashFS

char InitFlash(void)
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
        // no FlashFS Data Block found
        return -1;
        //EE_format();
        //InitFlash();
    }
    else
    {
        // read first Block
        EE_read_block(Flash.eprom.Block[0].adress, &Flash.Data, EE_Blocksize);
        
        // check the crc32
        crc     = crc16(&Flash.Data, EE_Blocksize-4);
        crc_r   = 0;
        crc_r   |= ((unsigned char) Flash.Data[FFS_Data_CRC]) << 8;
        crc_r   |= ((unsigned char) Flash.Data[FFS_Data_CRC+1]);

        if(crc == crc_r)
        {            
            /*if(Flash.Data[FFS_ext_devices]>0)
            {
                for(i=0; i < Flash.Data[FFS_ext_devices] ; i++)
                {
                    unsigned char type,dadr,adr;
                    char err;
                    
                    adr = Flash.Data[FFS_Device0 + i];
                    dadr = adr & 0xA0;
                    type = adr & 0xF0;
                    err = adr;
                    
                    switch(type)
                    {
                        case I2C_EEPROM:
                            #ifdef MOD_FlashFS_extI2C
                            err = i2c_chip_init(dadr, I2C_Delay_100khz, I2C_3Ext_8Int, 0);
                            #endif
                            break;
                        case I2C_EEPROM_16:
                            #ifdef MOD_FlashFS_extI2C
                            err = i2c_chip_init(dadr, I2C_Delay_100khz, I2C_0Ext_16Int, 0);
                            #endif
                            break;
                        default:
                            break;
                    }
                }
            }*/
        }
        else
        {
            // FlashFS Data Block invalid
            return -2;
            //EE_format();
            //InitFlash();            
        }
        
     /*********************** DDB (Device Data Block) Structure ****************\
     * Device Data                                                              *
     * Byte 0   : 32bit : Delay Value of this Device                            *
     *                      tells us how fast the device can go                 *
     * Byte 4   : 32bit : number of 64 byte blocks on the device                *
     *                      up to 255 Gigabytes per Device (whoa!)              *
     * Byte 8   : 32bit : number of used Blocks                                 *
     * Byte 12  : 32bit : start of CRC-Block                                    *
     *                      if zero   : none                                    *
     *                      else      : store 16 bit Checksum for each Block    *
     *                                  takes one CRC-Block for 32 Data Blocks  *
     * Byte 16  : 16bit : Maximum transfer size for multibyte write             *
     * Byte 18  :                                                               *
     * Byte 61  : 16bit : CRC for Block 0                                       *
     * Byte 63  : 8bit  : FlashFS Signature Byte                                *
     \**************************************************************************/
        
        Flash.eprom.Blocks = Flash.Data[FFS_int_blocks];
        d_cr();
        d_print("EEPROM: \0");
        d_value( Flash.eprom.UsedBlocks);
        d_print("/\0");
        d_value( Flash.eprom.Blocks );
        d_print(" Used\n");
    }
}



#endif
