#include "./DevelOS.h"
#ifdef MOD_FlashFS

// <editor-fold defaultstate="collapsed" desc="void EE_format(void)">
void EE_format(void)
{
    unsigned char i;
    
    // First, Overwrite all with Free sig
    for(i=0;i<EE_Blocksize;i++)
    {
        Flash.Data[i]=EE_sig_Free;
    }
    
    // now, write this empty but valid block to all blocks
    for(i=0;i<EE_Blocks;i++)
    {
        EE_save_block(i);
        //EE_write_block(Flash.eprom.Block[i].adress, &temp, EE_Blocksize);
    }
    
    // Now, prepare FlashFS Data
    for(i=0;i<EE_Blocksize;i++)
    {
        Flash.Data[i]=0;
    }
    
        // TODO: generalize, these are for testing
    Flash.Data[0]               = EE_Blocks;    // we have 16*64=1024 bytes on a 18F 46K20
    Flash.Data[1]               = 0;            // have 2 external eeproms on i2c
    Flash.Data[2]               = 0xB0;         // Atmel 32 kbit
    Flash.Data[3]               = 0xA8;         // ST 8 kbit
    Flash.Data[FFS_signature]   = EE_sig_FlashFS;
    // Write FlashFS Data
    EE_save_block(0);
    //EE_write_block(Flash.eprom.Block[0].adress, &temp, EE_Blocksize);
    
    // Finally, write OS Data
    SaveEEPROM_OS(1);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="char EE_load_block(unsigned char block)">
char EE_load_block(unsigned char block)
{
    unsigned char len = EE_Blocksize;
	unsigned char *ram_start_address = &Flash.Data;
    unsigned int eeprom_start_address = Flash.eprom.Block[block].adress;
    unsigned char result;
    unsigned int crc, crc_l;
    
    result = EE_read_block(eeprom_start_address, ram_start_address, len);
    
    crc         = crc16(&Flash.Data, EE_Blocksize-4);
    crc_l       = 0;
    crc_l      |= ((unsigned char) Flash.Data[FFS_Data_CRC]) << 8;
    crc_l      |= ((unsigned char) Flash.Data[FFS_Data_CRC+1]);
    
    if(crc != crc_l)
    {
        // block invalid
        return -1;
    }
    else
    {
        return 0;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="char EE_save_block(unsigned char block)">
char EE_save_block(unsigned char block)
{
    unsigned char len = EE_Blocksize;
	unsigned char *ram_start_address = &Flash.Data;
    unsigned int eeprom_start_address = Flash.eprom.Block[block].adress;
    char result;
    unsigned int crc;
    crc = crc16(&Flash.Data, EE_Blocksize-4);
    Flash.Data[FFS_Data_CRC]   = (unsigned char)( (crc >> 8) & 0xFF);
    Flash.Data[FFS_Data_CRC+1] = (unsigned char)( crc & 0xFF);    
    
    result = EE_write_block(eeprom_start_address, ram_start_address, len);

    return result;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char EE_read_block(unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len)">
unsigned char EE_read_block(unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len)
{
	unsigned char count;
	unsigned char *ram_start_address_copy;
	ram_start_address_copy = ram_start_address;

	//Write the address to eeadr
	EEADR = (eeprom_start_address & 0x00ff);		//(n.b. silicon bug for some devices - don't do directly before setting eecon1,rd)
	#ifdef EE_1k
		EEADRH = (eeprom_start_address >> 8);
	#endif

	//Set control bits
	EECON1bits.EEPGD = 0;							//Point to eeprom not program flash memory
	EECON1bits.CFGS = 0;							//Access eeprom registers, not configuration registers

	//Get the data from eeprom
	for (count = 0; count < len; count++)
	{
		EECON1bits.RD = 1;
		Nop();

		*ram_start_address++ = EEDATA;
		
		EEADR++;									//(n.b. silicon bug - don't do directly before setting eecon1,rd)
		#ifdef EE_1k
			if (EEADR == 0)
				EEADRH++;
		#endif
	}

	//Exit if not doing checksum bytes
	return 1;

}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char EE_read_byte(unsigned int eeprom_address)">
unsigned char EE_read_byte(unsigned int eeprom_address)
{
	unsigned char byte;

        byte = 0;
	//Write the address to eeadr
	EEADR = (eeprom_address & 0x00ff);		//(n.b. silicon bug for some devices - don't do directly before setting eecon1,rd)
	#ifdef EE_1k
		EEADRH = (eeprom_address >> 8);
	#endif

	//Set control bits
	EECON1bits.EEPGD = 0;							//Point to eeprom not program flash memory
	EECON1bits.CFGS = 0;							//Access eeprom registers, not configuration registers

	//Get the data from eeprom
        EECON1bits.RD = 1;
	Nop();

	byte = EEDATA;
        return byte;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char EE_write_block(unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len)">
unsigned char EE_write_block(unsigned int eeprom_start_address, unsigned char *ram_start_address, unsigned char len)
{
    unsigned char count,flag;
    flag=0;

    //Write the address to eeadr
    EEADR = (eeprom_start_address & 0x00ff);		//(n.b. silicon bug - don't do directly before setting eecon1,rd)
    #ifdef EE_1k
    EEADRH = (eeprom_start_address >> 8);
    #endif

    //Set control bits
    EECON1bits.EEPGD = 0;							//Point to eeprom not program flash memory
    EECON1bits.CFGS = 0;							//Access eeprom registers, not configuration registers
    EECON1bits.WREN = 1;							//Enable eeprom writes

    // save interrupt state
    if(INTCONbits.GIEH)
    {
        flag=2;
    }
    if(INTCONbits.GIEL)
    {
        flag++;
    }
    
    //disable interrupt
    INTCONbits.GIEL=0;
    INTCONbits.GIEH=0;
    PIR2bits.EEIF = 0;      //Clear write complete flag
    
    //Write the data to eeprom
    for (count = 0; count < len; count++)
    {
        EEDATA = *ram_start_address++;
        EECON2 = 0x55;			//Do the write enable sequence
        EECON2 = 0xaa;
        EECON1bits.WR = 1;		//Start write
        while( EECON1bits.WR ); //Wait for write to complete
        PIR2bits.EEIF = 0;      //Clear write complete flag
        EEADR++;				//Increment address

#ifdef EE_1k
        if (EEADR == 0)
        EEADRH++;
#endif
    }
    //restore injterrupt states
    if(flag>=2)
    {
        INTCONbits.GIEH=1;
        flag -=2;
    }
    if(flag>0)
    {
        INTCONbits.GIEL=1;
    }
    return 0;
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="TODO: check_block">
/*unsigned char EE_check_block(unsigned int eeprom_start_address, unsigned char len)
{
	unsigned int check;
	unsigned char count;

	//Write the address to eeadr
	EEADR = (eeprom_start_address & 0x00ff);		//(n.b. silicon bug for some devices - don't do directly before setting eecon1,rd)
	#ifdef EE_1k
		EEADRH = (eeprom_start_address >> 8);
	#endif

	check = (unsigned int) 1;


	//Set control bits
	EECON1bits.EEPGD = 0;							//Point to eeprom not program flash memory
	EECON1bits.CFGS = 0;							//Access eeprom registers, not configuration registers

	//Get the data from eeprom
	for (count = 0; count < len; count++)
	{
		EECON1bits.RD = 1;
		Nop();

		check += EEDATA;

		EEADR++;									//(n.b. silicon bug - don't do directly before setting eecon1,rd)
		#ifdef EE_1k
			if (EEADR == 0)
				EEADRH++;
		#endif
	}

	//----- DO THE CHECKSUM -----
	//Get and check checksum h
	EECON1bits.RD = 1;
	Nop();

	if (EEDATA != (unsigned char) (check >> 8))
		goto read_pic_eeprom_fail;

	//Get and check checksum l
	EEADR++;										//(n.b. silicon bug - don't do directly before setting eecon1,rd)
	#ifdef EE_1k
		if (EEADR == 0)
			EEADRH++;
	#endif

	EECON1bits.RD = 1;
	Nop();

	if (EEDATA != (unsigned char) (check & 0x00ff))
		goto read_pic_eeprom_fail;


	return 1;										//Checksum good

read_pic_eeprom_fail:
	//----- BAD CHECKSUM -----

	return 0;
}*/
// </editor-fold> // TODO: durch sinnvolle check-funktion ersetzen

// <editor-fold defaultstate="collapsed" desc="void InitEEPROM(void)">
void InitEEPROM(void)
{
    char i;

    for(i=0;i<EE_Blocks;i++)
    {
        Flash.eprom.Block[i].adress=i*EE_Blocksize;
        Flash.eprom.Block[i].signature=EE_read_byte(Flash.eprom.Block[i].adress + (EE_Blocksize-1) ) ;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char LoadEEPROM_OS(unsigned char block)">
char LoadEEPROM_OS(unsigned char block)   // Read System Data from EEPROM
{
//    unsigned char temp[EE_Blocksize];
//    unsigned int crc, crc_l;
    signed char err;

    err = EE_load_block(block);
    if(err != 0)
    {
        return err;
    }
        

    // EE_read_block(Flash.eprom.Block[block].adress, &temp[0], EE_Blocksize);
//    crc         = crc16(&temp, EE_Blocksize-4);
//    crc_l       = 0;
//    crc_l      |= ((unsigned char) temp[61]) << 8;
//    crc_l      |= ((unsigned char) temp[62]);
//    if(crc != crc_l)
//    {
//        // block invalid
//        return -1;
//    }
    else
    {
        //OS.prev_runlevel    = temp[0];
        //OS.runlevel         = temp[1];
        //OS.runmode          = temp[2];
        //OS.CPUClock         = temp[3];
        OS.Temp             = Flash.Data[4];
        OS.U12V             = Flash.Data[5];
        OS.U3V3             = Flash.Data[6];
        OS.U5V0             = Flash.Data[7];
        OS.Rcount           += Flash.Data[8];
        OS.RcountBOR        += Flash.Data[9];
        OS.RcountPOR        += Flash.Data[10];
        OS.RcountRI         += Flash.Data[11];
        OS.RcountWDT        += Flash.Data[12];
        OS.RcountSO         += Flash.Data[13];
        OS.RcountSU         += Flash.Data[14];
#ifdef MOD_rtc
        rtc.days            = Flash.Data[15];
        rtc.hour            = Flash.Data[16];
        rtc.mins            = Flash.Data[17];
        rtc.mont            = Flash.Data[18];
        rtc.secs            = Flash.Data[19];
        rtc.year            = Flash.Data[20];
#endif
        return 0;   // block loaded
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char SaveEEPROM_OS(unsigned char block)">
unsigned char SaveEEPROM_OS(unsigned char block)   // Write System Data to EEPROM
{
//    unsigned char temp[EE_Blocksize];
//    unsigned int crc;
    memset(Flash.Data, 0, EE_Blocksize );

    Flash.Data[0]     = OS.prev_runlevel;
    Flash.Data[1]     = OS.runlevel;
    Flash.Data[2]     = OS.runmode;
    Flash.Data[3]     = 0;                // OS.CPUClock; is 4 bytes
    Flash.Data[4]     = OS.Temp;
    Flash.Data[5]     = OS.U12V;
    Flash.Data[6]     = OS.U3V3;
    Flash.Data[7]     = OS.U5V0;
    Flash.Data[8]     = OS.Rcount;
    Flash.Data[9]     = OS.RcountBOR;
    Flash.Data[10]    = OS.RcountPOR;
    Flash.Data[11]    = OS.RcountRI;
    Flash.Data[12]    = OS.RcountWDT;
    Flash.Data[13]    = OS.RcountSO;
    Flash.Data[14]    = OS.RcountSU;
#ifdef MOD_rtc
    Flash.Data[15]    = rtc.days;
    Flash.Data[16]    = rtc.hour;
    Flash.Data[17]    = rtc.mins;
    Flash.Data[18]    = rtc.mont;
    Flash.Data[19]    = rtc.secs;
    Flash.Data[20]    = rtc.year;
#endif
    Flash.Data[63]    = EE_sig_System;

    EE_save_block(block);
    
    //EE_write_block(Flash.eprom.Block[block].adress, &temp[0], EE_Blocksize);    
}// </editor-fold>
#endif    /* MOD_FlashFS */
