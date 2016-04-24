#include "uart.h"

char setBaud(char cmd)                  // TODO: set new baudrate
{
/*    char tmp=0;
    char buff[20];
    char strbuff[3];

    while(xmit(BDstring[8]));
    tmp=atoi( &command[strlen(cmdlist[cmd])] );
    if( (tmp > 0) && (tmp < 8) )
    {
        while(xmit(BDstring[tmp]));
        for(int i=0;i<1000;i++)
        {
            delayLong(1);
        }
        SPBRGH=BAUD[tmp][1];
        SPBRG=BAUD[tmp][2];
        config[1]=tmp;
    }else if( (command[strlen(cmdlist[cmd])]=='p') && (config[1]>0) && (config[1]<8) )
    {
        delayLong(100000);
        INTCONbits.GIEH=0;
        INTCONbits.GIEL=0;
        Write_b_eep( 17 , config[1] ); // write to EEPROM
        Busy_eep ();
        delayLong(100000);
        INTCONbits.GIEH=1;
        INTCONbits.GIEL=1;
        while(xmit("permanent\r"));
    }
    else {
        while(xmit(BDstring[0]));
        while(xmit(BDstring[9]));
        for(int i=1;i<8;i++)
        {
            buff[0]=0;
            strbuff[0]=0;
            strcat (&buff, "  ");
            itoa (&strbuff, i, 16);
            strcat (&buff, strbuff);
            strcat (&buff, " : ");
            strcat (&buff, BDstring[i]);
            strcat (&buff, " BPS\r");
            while(xmit(buff));
        }
    }*/
}

char sendString(const char* string[], char len)
{
    if(uart.busy == 1)
    {
        return -1;
    }
    else if(len>TX_BUFF_SIZE)
    {
        return -2;
    }
    else
    {
        // copy string to tx buffer
        //strcpy( &uart.tx_buff, string );
        uart.busy = 1;
        uart.tx_bytes = len;
        addEvent(EV_uart_tx, 0);  
    }
    return 0;
}