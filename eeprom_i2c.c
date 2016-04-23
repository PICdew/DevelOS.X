#include "DevelOS.h"

#ifdef MOD_FlashFS
#ifdef MOD_FlashFS_extI2C


char i2c_chip_init(unsigned int     Daddr,          // device bus base address
                    unsigned long   Delay,          // timing parameter for this device
                    unsigned char   AddressMode,    // how to adress this device
                    unsigned char   MBWsize         // how many bytes can be written sequencially
)
{
    int crc,crc_t,result;
    unsigned char byte,err,i;
    unsigned long delay_t;
    
    i2c_chip.AddressMode=AddressMode;
    i2c_chip.Daddr=Daddr;
    i2c_chip.Delay=Delay;
    i2c_chip.MBWsize=MBWsize;
    
    result = I2C_eeReadByte(i2c_chip, I2C_DDB_sig);
    byte = result & 0x00FF;
    err = result >>8; 

    if( err != 0xf0)
    {
        if(byte == EE_sig_FlashFS)
        {
            err = I2C_eeReadBlock(i2c_chip, 0, &i2c_bus.Buffer);
            crc = i2c_bus.Buffer[I2C_DDB_crc];
            crc = crc << 8;
            crc |= (i2c_bus.Buffer[I2C_DDB_crc]&0x00FF);
            crc_t = crc16(&i2c_bus.Buffer[I2C_DDB_crc], EE_Blocksize-4);
            if(crc == crc_t)
            {
                //valid data Block
                for(i=0;i<4;i++)
                {
                    delay_t += i2c_bus.Buffer[I2C_DDB_delay+i];
                    delay_t = delay_t << 8;            
                }
                i2c_chip.Delay=delay_t;
                i2c_chip.MBWsize=i2c_bus.Buffer[I2C_DDB_mbw];
                return 1;
            }
            else
            {
                return -1;  // Device Data Block corrupted
            }
        }
        else
        {
            return -2;      // chip unformated
        }
    }
    else
    {
            return -3;      // invalid chip or bus error
    }
}

// <editor-fold defaultstate="collapsed" desc="unsigned int I2C_eeReadByte(struct I2Ceeprom device, unsigned int b_adr)">
unsigned int I2C_eeReadByte(struct I2Ceeprom device, unsigned int b_adr)
{
    unsigned char ack, dadr, badr, adr;
    unsigned int byte;
    byte =0;

    // set speed for this device
    i2c_bus.Delay = device.Delay;
    
    switch(device.AddressMode)
    {
        case I2C_3Ext_8Int:     // <editor-fold defaultstate="collapsed" desc="I2C_3Ext_8Int">
        // start 
        I2C_Start();

        // determine device address
        dadr = (b_adr & 0b01110000) >> 7;    // take 3 bits from byte adress into device adress
        dadr = dadr | device.Daddr;           // OR in the base adress

        // determine byte address
        badr = (char) b_adr & 0x00FF;        // mask only the lower 8 bits
        
        // write d_adr, wait for ack
        ack = I2C_ByteOut(dadr);
        if(ack==0)
        {
            byte = 0xF000;
        }
        
        // write b_adr, wait for ack
        ack = I2C_ByteOut(adr);
        if(ack==0)
        {
            byte = 0xF000;
        }
        
        // another start
        I2C_Start();

        dadr = dadr | 0b00000001;             // OR in the R/W bit
        // write d_adr, wait for ack
        ack = I2C_ByteOut(dadr);
        if(ack==0)
        {
            byte = 0xF000;
        }
        
        // read byte, send nack
        byte = byte | I2C_ByteIn(0);

        // stop
        I2C_Stop(); //</editor-fold>
            break;
        case I2C_0Ext_16Int:    // <editor-fold defaultstate="collapsed" desc="I2C_0Ext_16Int">
        // start 
        I2C_Start();

        // determine address
        adr = device.Daddr & 0x00AF;

        // write d_adr, wait for ack
        ack = I2C_ByteOut(adr);
        if(ack==0)
        {
            byte = 0xF000;
        }

        // write upper b_adr, wait for ack
        badr = (unsigned char) (b_adr >> 8);
        ack = I2C_ByteOut(b_adr);
        if(ack==0)
        {
            byte = 0xF000;
        }
        
        // write lower b_adr, wait for ack
        badr = (unsigned char) (b_adr & 0x00FF);
        ack = I2C_ByteOut(b_adr);
        if(ack==0)
        {
            byte = 0xF000;
        }
        
        // another start
        I2C_Start();

        dadr = dadr | 0b00000001;             // OR in the R/W bit
        
        // write d_adr, wait for ack
        ack = I2C_ByteOut(dadr);
        if(ack==0)
        {
            byte = 0xF000;
        }
        // read byte, send nack
        byte = byte | I2C_ByteIn(0);

        // stop
        I2C_Stop();//</editor-fold>
            break;
        default:
            break;
    }

    // return byte
    return byte;
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char I2C_eeWriteByte(struct I2Ceeprom device, unsigned int b_adr, unsigned char byte)">
unsigned char I2C_eeWriteByte(struct I2Ceeprom device, unsigned int b_adr, unsigned char byte)
{
    unsigned char ack,dadr,badr;
    unsigned int tbyte;
    tbyte =0;

    // set speed for this device
    //i2c_bus.Delay = device.Delay;
    // start 
    
    // write d_adr, wait for ack
    // write b_adr, wait for ack
    // write byte, wait for ack
    // stop
    // return error code
}
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char I2C_eeReadBlock(struct I2Ceeprom device, unsigned long block, unsigned char *ram_start_address)">
char I2C_eeReadBlock(struct I2Ceeprom device, unsigned long block, unsigned char *ram_start_address)
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
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char I2C_eeWriteBlock(struct I2Ceeprom device, unsigned long block, unsigned char *ram_start_address)">
unsigned char I2C_eeWriteBlock(struct I2Ceeprom device, unsigned long block, unsigned char *ram_start_address)
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
//</editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char I2C_eeCheckBlock(struct I2Ceeprom device, unsigned long block)">
unsigned char I2C_eeCheckBlock(struct I2Ceeprom device, unsigned long block)
{
    // determine address of CRC on device
    // determine start address of block
    // read CRC from device
    // read Block and calculate CRC
    // compare CRCs and return error code
}
//</editor-fold>

#endif /* MOD_FlashFS_extI2C */
#endif /* MOD_FlashFS */
