#include "uart.h"

        #ifdef MOD_UART

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

char sendString(const char string[], char len)
{
    char i;
    if(uart.busy == 1)
    {
        return -1;
    }
    else if(len > TX_BUFF_SIZE)
    {
        return -2;
    }
    else if(TXSTAbits.TXEN)
    {
        return -3;
    }
    else
    {
        // copy string to tx buffer
        //strncpy( uart.tx_buff, string, len );
        for(i=0;i<len;i++)
        {
            uart.tx_buff[i]=string[i];
        }
//        uart.tx_buff[i]=0x0D;
//        uart.tx_buff[i+1]=0x0A;
//        len += 2;
        uart.busy = 1;
        uart.tx_bytes = len;
        uart.tx_byte=0;
        //PIE1bits.TXIE=1;
        TXSTAbits.TXEN=1;
        return 0;
    }
    
}

void clearBuffTX(void)
{
    for(uart.tx_bytes=0; uart.tx_bytes<TX_BUFF_SIZE; uart.tx_bytes++)
    {
        uart.tx_buff[uart.tx_bytes]=0;
    }
    uart.tx_bytes=0;
}

char uart_txByte(void)                         // called from irq-handler. read byte from tx buffer
{
    char i;
    char tmp=0;
    
    if(uart.tx_bytes>0)
    {
        tmp=uart.tx_buff[0];
        // it would be nicer to have the following done outside interrupt scope,
        // yet i'm not sure if this could lead to missed and or double-send bytes
        for(i=0;i<TX_BUFF_SIZE-1;i++)
        {
            uart.tx_buff[i]=uart.tx_buff[i+1];
        }
        uart.tx_bytes--;
    }
    return tmp;
}

char xmit(const char * string)          // send string to uart
{
    signed char i,res;
    char bytes=strlen(string);

    if(strlen(string)>TX_BUFF_SIZE)
    {
        res= -1;                      // string too long, hopeless to retry
    }
    else if(TXSTAbits.TXEN==1)
    {
        res= 2;                       // uart already running, retry later
    }
    else if(uart.tx_bytes+strlen(string)>TX_BUFF_SIZE)
    {
        res = 1;                       // insufficient buffer space, retry later
    }
    else
    {
        for(i=0; i < bytes;i++)    // copy string into uart buffer, and start sending
        {
            uart.tx_buff[uart.tx_bytes++] = *string++;
        }
        TXSTAbits.TXEN=1;
        res= 0;
    }
    return res;
}

char uart_xmit(const char * string, unsigned char len)          // send string to uart
{
    signed char i,res;
    unsigned char bytes=len;

    if(bytes>TX_BUFF_SIZE)
    {
        res= -1;                      // string too long, hopeless to retry
    }
    else if(TXSTAbits.TXEN==1)
    {
        res= 2;                       // uart already running, retry later
    }
    else if(uart.tx_bytes+bytes>TX_BUFF_SIZE)
    {
        res = 1;                       // insufficient buffer space, retry later
    }
    else
    {
        for(i=0; i < bytes;i++)    // copy string into uart buffer, and start sending
        {
            uart.tx_buff[uart.tx_bytes++] = *string++;
        }
        TXSTAbits.TXEN=1;
        res= 0;
    }
    return res;
}
#endif 

