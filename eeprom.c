#include "./DevelOS.h"
#ifdef MOD_FlashFS

// <editor-fold defaultstate="collapsed" desc="void EE_format(void)">
void EE_format(void)
{
	unsigned char temp[EE_Blocksize];
    unsigned int crc;
    unsigned char byte;
    unsigned char i;
    
    // First, Overwrite all with Free sig


    d_cr();
    d_print("Formating ...\0");
    RefreshDisplay(); 
    for(i=0;i<EE_Blocksize;i++)
    {
        temp[i]=EE_sig_Free;
    }
    temp[EE_Blocksize-2]=0;
    for(i=0;i<EE_Blocks;i++)
    {
        if(i==0)
        {
            temp[EE_Blocksize-1]=EE_sig_FlashFS;
        }
        else
        {
            temp[EE_Blocksize-1]=EE_sig_Free;
        }
        EE_write_block(Flash.eprom.Block[i].adress, &temp, EE_Blocksize);
    }
    
    // Now, prepare FlashFS Data
    // TODO: generalize, these are for testing
    for(i=0;i<EE_Blocksize;i++)
    {
        temp[i]=0;
    }
    temp[0]                 = 16;           // we have 16*64=1024 bytes on a 18F 46K20
    temp[1]                 = 0;            // have 2 external eeproms on i2c
    temp[2]                 = 0xB0;         // Atmel 32 kbit
    temp[3]                 = 0xA8;         // ST 8 kbit
    crc                     = crc16(&temp, EE_Blocksize-4);
    byte                    = (unsigned char)( (crc >> 8) & 0xFF);
    temp[FFS_Data_CRC]      = byte;
    byte                    = (unsigned char)( crc & 0xFF);
    temp[FFS_Data_CRC+1]    = byte;
    temp[FFS_signature]     = EE_sig_FlashFS;
    // Write FlashFS Data
    EE_write_block(Flash.eprom.Block[0].adress, &temp, EE_Blocksize);
    
    // Finally, write OS Data
    SaveEEPROM_OS(1);
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


    //Write the data to eeprom
    for (count = 0; count < len; count++)
    {
        EEDATA = *ram_start_address++;

        PIR2bits.EEIF = 0;							//Clear write complete flag

        if(INTCONbits.GIEH)
        {
            flag=2;
        }
        if(INTCONbits.GIEL)
        {
            flag++;
        }
        INTCONbits.GIEL=0;
        INTCONbits.GIEH=0;

        EECON2 = 0x55;								//Do the write enable sequence
        EECON2 = 0xaa;
        EECON1bits.WR = 1;							//Start write

        
        
        if(flag>=2)
        {
            INTCONbits.GIEH=1;
            flag -=2;
        }
        if(flag>0)
        {
            INTCONbits.GIEL=1;
        }
        flag=0;

        while (PIR2bits.EEIF == 0);					//Wait for write to complete (do it above as faster)    

        EEADR++;									//Increment address

#ifdef EE_1k
        if (EEADR == 0)
        EEADRH++;
#endif
    }
    return 1;
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

    //Flash.eprom.FreeBlocks=0;

    d_print("LESE EPROM\n");
    //VFLD.Symbol[VFLD_CN2]=1;
    RefreshDisplay();

    for(i=0;i<EE_Blocks;i++)
    {
        Flash.eprom.Block[i].adress=i*EE_Blocksize;
        Flash.eprom.Block[i].signature=EE_read_byte(Flash.eprom.Block[i].adress + (EE_Blocksize-1) ) ;
        if(Flash.eprom.Block[i].signature==0xAA | Flash.eprom.Block[i].signature == 0xFF)
        {
            //Flash.eprom.FreeBlocks++;
        }
        //Flash.eprom.Block[i].used=EE_read_byte(Flash.eprom.Block[i].adress + (EE_Blocksize-2) ) ;
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char LoadEEPROM_OS(unsigned char block)">
unsigned char LoadEEPROM_OS(unsigned char block)   // Read System Data from EEPROM
{
    unsigned char temp[EE_bytes_System];

    EE_read_block(Flash.eprom.Block[block].adress, &temp[0], EE_bytes_System);
        OS.prev_runlevel    = temp[0];
        OS.runlevel         = temp[1];
        OS.runmode          = temp[2];
        //OS.CPUClock         = temp[3];
        OS.Temp             = temp[4];
        OS.U12V             = temp[5];
        OS.U3V3             = temp[6];
        OS.U5V0             = temp[7];
        OS.Rcount           = temp[8];
        OS.RcountBOR        = temp[9];
        OS.RcountPOR        = temp[10];
        OS.RcountRI         = temp[11];
        OS.RcountWDT        = temp[12];
        OS.RcountSO         = temp[13];
        OS.RcountSU         = temp[14];
#ifdef MOD_rtc
        rtc.days            = temp[15];
        rtc.hour            = temp[16];
        rtc.mins            = temp[17];
        rtc.mont            = temp[18];
        rtc.secs            = temp[19];
        rtc.year            = temp[20];
#endif
        return 1;   // block loaded
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="unsigned char SaveEEPROM_OS(unsigned char block)">
unsigned char SaveEEPROM_OS(unsigned char block)   // Write System Data to EEPROM
{
    unsigned char temp[EE_bytes_System];

    temp[0]     = OS.prev_runlevel;
    temp[1]     = OS.runlevel;
    temp[2]     = OS.runmode;
    temp[3]     = 0;                // OS.CPUClock; is 4 bytes
    temp[4]     = OS.Temp;
    temp[5]     = OS.U12V;
    temp[6]     = OS.U3V3;
    temp[7]     = OS.U5V0;
    temp[8]     = OS.Rcount;
    temp[9]     = OS.RcountBOR;
    temp[10]    = OS.RcountPOR;
    temp[11]    = OS.RcountRI;
    temp[12]    = OS.RcountWDT;
    temp[13]    = OS.RcountSO;
    temp[14]    = OS.RcountSU;
#ifdef MOD_rtc
    temp[15]    = rtc.days;
    temp[16]    = rtc.hour;
    temp[17]    = rtc.mins;
    temp[18]    = rtc.mont;
    temp[19]    = rtc.secs;
    temp[20]    = rtc.year;
#else
    temp[15]    = 0;
    temp[16]    = 0;
    temp[17]    = 0;
    temp[18]    = 0;
    temp[19]    = 0;
    temp[20]    = 0;
#endif
    EE_write_block(Flash.eprom.Block[block].adress, &temp[0], EE_bytes_System);

    temp[0]     = EE_bytes_System;
    temp[1]     = EE_sig_System;
    
    EE_write_block(Flash.eprom.Block[block].adress + (EE_Blocksize-2), &temp[0], 2);
}// </editor-fold>
#endif    /* MOD_FlashFS */
