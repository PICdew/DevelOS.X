#include "DevelOS.h"
#include "i2c_bitbang.h"

    void I2C_Init(void)             // <editor-fold defaultstate="collapsed" desc="void I2C_Init(void)">
    {
        char i;
        
        i2c_bus.BufPos =0;
        i2c_bus.Delay = 0;          // calculate for 100kHz here
        for(i=0; i<I2C_Buffer;i++)
        {
            i2c_bus.Buffer[i]=0;
        }
    }   
    //</editor-fold>
    
    void I2C_Start(void)            // <editor-fold defaultstate="collapsed" desc="void I2C_Start(void)">
    // do the start condition
    {
        SDA_t   =0;
        SCL_t   =0;
        SDA     =1;
        SCL     =1;
        // wait a moment ...
        OS_delay_ns(i2c_bus.Delay);
        SDA     =0;
        // wait another moment
        OS_delay_ns(i2c_bus.Delay);
        SCL     =0;
        // start done
    } // </editor-fold>
    
    void I2C_Stop(void)             // <editor-fold defaultstate="collapsed" desc="void I2C_Stop(void)">   
    // do the stop condition
    {
        SDA_t   =0;
        SCL     =1;
        // wait
        OS_delay_ns(i2c_bus.Delay);
        SDA     =1;
        //wait
        OS_delay_ns(i2c_bus.Delay);
        // stop done
    }//</editor-fold>

    unsigned char I2C_ByteIn(char ack)       // <editor-fold defaultstate="collapsed" desc="char I2C_ByteIn(char ack)">
    // Read a byte from the bus, maybe ack it, return the byte
    {
        unsigned char byte,i;
        byte =0;
        
        // set SDA input
        SDA_t = 1;      
        
        //clock in 8 bits
        for(i=0;i<8;i++)
        {
            SCL = 1;
            OS_delay_ns(i2c_bus.Delay);
            if(SDA)
            {
                byte++;
            }
            byte = byte << 1;
            SCL = 0;
            OS_delay_ns(i2c_bus.Delay);
        }
        
        // set SDA output
        SDA_t = 0;
        
        // set ack/nack
        if(ack==1)
        {
            SDA = 0;
        }
        else
        {
            SDA = 1;
        }
        
        // clock out the bit
        SCL = 1;
        OS_delay_ns(i2c_bus.Delay);
        SCL = 0;
        
        // done, return the byte
        return byte;
    }//</editor-fold>
        
    char I2C_ByteOut(char byte);    // <editor-fold defaultstate="collapsed" desc="char I2C_ByteOut(char byte)">
    // Write a byte to the bus, return the ack
    //</editor-fold>
    