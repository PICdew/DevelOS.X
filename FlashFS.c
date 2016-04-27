
#include "./DevelOS.h"

#ifdef MOD_FlashFS

char InitFlash(void)
{
    char i;
    signed char err;
    
    // first, init all data to safe values
    Flash.eprom.UsedBlocks=0;
    Flash.eprom.Blocks=EE_Blocks;

    for(i=0; i<EE_Blocksize; i++)
    {
        Flash.Data[i]=0;    
    }
    
    // set addresses and read signature of each block
    InitEEPROM();
    
    if(Flash.eprom.Block[0].signature != EE_sig_FlashFS)
    {
        // no FlashFS Data Block found
        return -2;
    }
    else
    {
        // block 0 seems to be FlashFS Data Block.
        err = EE_load_block(0); 
        if(err != 0)
            return err;     // something went wrong
        
        // seems we have valid FlashFS data
        Flash.eprom.Blocks = Flash.Data[FFS_int_blocks];
        
        if(Flash.eprom.Blocks != EE_Blocks)
            return -3;      // this should not happen. never ever.
        
        // now, check for used blocks
        for(i=0; i<Flash.eprom.Blocks; i++)
        {
            if(Flash.eprom.Block[i].signature != EE_sig_Free)
            {
                Flash.eprom.UsedBlocks++;
            }
        }
        
        // TODO: read info from external devices
        
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
        
        // all fine
        return 0;
    }
}


#endif


/*  old code
 * // read first Block
//        EE_read_block(Flash.eprom.Block[0].adress, &Flash.Data, EE_Blocksize);
//        
//        // check the crc32
//        crc     = crc16(&Flash.Data, EE_Blocksize-4);
//        crc_r   = 0;
//        crc_r   |= ((unsigned char) Flash.Data[FFS_Data_CRC]) << 8;
//        crc_r   |= ((unsigned char) Flash.Data[FFS_Data_CRC+1]);
//
//        if(crc == crc_r)
//        {            
////            if(Flash.Data[FFS_ext_devices]>0)
////            {
////                for(i=0; i < Flash.Data[FFS_ext_devices] ; i++)
////                {
////                    unsigned char type,dadr,adr;
////                    char err;
////                    
////                    adr = Flash.Data[FFS_Device0 + i];
////                    dadr = adr & 0xA0;
////                    type = adr & 0xF0;
////                    err = adr;
////                    
////                    switch(type)
////                    {
////                        case I2C_EEPROM:
////                            #ifdef MOD_FlashFS_extI2C
////                            err = i2c_chip_init(dadr, I2C_Delay_100khz, I2C_3Ext_8Int, 0);
////                            #endif
////                            break;
////                        case I2C_EEPROM_16:
////                            #ifdef MOD_FlashFS_extI2C
////                            err = i2c_chip_init(dadr, I2C_Delay_100khz, I2C_0Ext_16Int, 0);
////                            #endif
////                            break;
////                        default:
////                            break;
////                    }
////                }
////            }
//        }
//        else
//        {
//            // FlashFS Data Block invalid
//            return -2;
//            //EE_format();
//            //InitFlash();            
//        }
 * 
 */