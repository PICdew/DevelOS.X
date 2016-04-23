#include "DevelOS.h"

#ifdef MOD_FlashFS_extI2C

unsigned int ReadByte(struct I2Ceeprom device, unsigned int b_adr)
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
        adr = device.Daddr ;

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

unsigned char WriteByte(unsigned char d_adr, unsigned int b_adr, unsigned char byte)
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

#endif /* MOD_FlashFS_extI2C */
