/*
 *
 * Copyright (c) hivision, Inc. 2006. All rights reserved.
 *
 */

/**
 * @file public.c
 * @Sample FIP user level program to display given string,
 * monitoring eject button and eject disc if needed.
 *
 * @author ZhangHongBo
 */

#include <p18cxxx.h>
#include <delays.h>
#include "./main.h"
#include "pt6311.h"


static int vfd_playstatus = 0;
static int vfd_linkstatus = 0;
static int vfd_pcusbstatus = 1;
static int vfd_repeatstatus = 0;
static int vfd_mainstatus = 0;

static int tv_mode = 2;

static int vfd_ON_OFF=0;

// address : 0x00 0x01
//////////////////////////////////////////////////////////////////////
static const rom int vfd_hour[10] [2] = {
		{DIGIT_L(0,1,0,1,0,1,0,0), DIGIT_H(0,1,0,1,0,1,1,0)},		//0
		{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,0,0,1,0)},		// 1
		{DIGIT_L(0,1,0,1,0,0,0,1), DIGIT_H(0,0,0,1,0,1,1,0)},		// 2	
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,0,0,1,0,1,1,0)},		// 3
		{DIGIT_L(0,0,0,0,0,1,0,1), DIGIT_H(0,1,0,1,0,0,1,0)},		// 4
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,1,0,0,0,1,1,0)},		// 5
		{DIGIT_L(0,1,0,1,0,1,0,1), DIGIT_H(0,1,0,0,0,1,1,0)},		// 6
		{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,0,1,1,0)},		// 7
		{DIGIT_L(0,1,0,1,0,1,0,1), DIGIT_H(0,1,0,1,0,1,1,0)},		// 8
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,1,0,1,0,1,1,0)}		// 9
};

static const rom int vfd_min_high[7][2] = {

		{DIGIT_L(1,0,1,0,1,0,0,0), DIGIT_H(1,0,1,0,1,0,1,0)},		//0
		{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,1,0,0,0,1,0)},		// 1
		{DIGIT_L(1,0,1,0,0,0,1,0), DIGIT_H(0,0,1,0,1,0,1,0)},		// 2	
		{DIGIT_L(1,0,0,0,1,0,1,0), DIGIT_H(0,0,1,0,1,0,1,0)},		// 3
		{DIGIT_L(0,0,0,0,1,0,1,0), DIGIT_H(1,0,1,0,0,0,1,0)},		// 4
		{DIGIT_L(1,0,0,0,1,0,1,0), DIGIT_H(1,0,0,0,1,0,1,0)},		// 5
		{DIGIT_L(1,0,1,0,1,0,1,0), DIGIT_H(1,0,0,0,1,0,1,0)}		// 6
};
// address : 0x03 0x04
//////////////////////////////////////////////////////////////////////
static const rom int vfd_min_low[10] [2] = {
		{DIGIT_L(0,1,0,1,0,1,0,0), DIGIT_H(0,1,0,1,0,1,1,0)},		//0
		{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,0,0,1,0)},		// 1
		{DIGIT_L(0,1,0,1,0,0,0,1), DIGIT_H(0,0,0,1,0,1,1,0)},		// 2	
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,0,0,1,0,1,1,0)},		// 3
		{DIGIT_L(0,0,0,0,0,1,0,1), DIGIT_H(0,1,0,1,0,0,1,0)},		// 4
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,1,0,0,0,1,1,0)},		// 5
		{DIGIT_L(0,1,0,1,0,1,0,1), DIGIT_H(0,1,0,0,0,1,1,0)},		// 6
		{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,0,1,1,0)},		// 7
		{DIGIT_L(0,1,0,1,0,1,0,1), DIGIT_H(0,1,0,1,0,1,1,0)},		// 8
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,1,0,1,0,1,1,0)}		// 9
};
		
static const rom int vfd_sec_high[7][2] = {

		{DIGIT_L(1,0,1,0,1,0,0,0), DIGIT_H(1,0,1,0,1,0,1,0)},		//0
		{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,1,0,0,0,1,0)},		// 1
		{DIGIT_L(1,0,1,0,0,0,1,0), DIGIT_H(0,0,1,0,1,0,1,0)},		// 2	
		{DIGIT_L(1,0,0,0,1,0,1,0), DIGIT_H(0,0,1,0,1,0,1,0)},		// 3
		{DIGIT_L(0,0,0,0,1,0,1,0), DIGIT_H(1,0,1,0,0,0,1,0)},		// 4
		{DIGIT_L(1,0,0,0,1,0,1,0), DIGIT_H(1,0,0,0,1,0,1,0)},		// 5
		{DIGIT_L(1,0,1,0,1,0,1,0), DIGIT_H(1,0,0,0,1,0,1,0)}		// 6
};
// address : 0x06 0x07
//////////////////////////////////////////////////////////////////////
static const rom int vfd_sec_low[10] [2] = {
		{DIGIT_L(0,1,0,1,0,1,0,0), DIGIT_H(0,1,0,1,0,1,0,0)},		//0
		{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,0,0,0,0)},		// 1
		{DIGIT_L(0,1,0,1,0,0,0,1), DIGIT_H(0,0,0,1,0,1,0,0)},		// 2	
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,0,0,1,0,1,0,0)},		// 3
		{DIGIT_L(0,0,0,0,0,1,0,1), DIGIT_H(0,1,0,1,0,0,0,0)},		// 4
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,1,0,0,0,1,0,0)},		// 5
		{DIGIT_L(0,1,0,1,0,1,0,1), DIGIT_H(0,1,0,0,0,1,0,0)},		// 6
		{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,0,1,0,0)},		// 7
		{DIGIT_L(0,1,0,1,0,1,0,1), DIGIT_H(0,1,0,1,0,1,0,0)},		// 8
		{DIGIT_L(0,1,0,0,0,1,0,1), DIGIT_H(0,1,0,1,0,1,0,0)}		// 9
};

static const rom int vfd_tvip[7][2] = {
		{DIGIT_L(1,0,1,0,1,0,0,0), DIGIT_H(1,0,1,0,1,0,0,1)},		//0 i
		{DIGIT_L(1,0,1,0,1,0,0,0), DIGIT_H(1,0,1,0,1,0,1,0)},		// 1 p	
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},		// 2 none
		{DIGIT_L(1,0,1,0,1,0,1,0), DIGIT_H(1,0,0,0,1,0,0,1)},          //6 i
		{DIGIT_L(1,0,1,0,1,0,0,0), DIGIT_H(1,0,1,0,1,0,0,0)},          // 0
		{DIGIT_L(0,0,0,0,1,0,1,0), DIGIT_H(1,0,1,0,0,0,0,0)},           // 4
		{DIGIT_L(1,0,1,0,1,0,1,0), DIGIT_H(1,0,1,0,1,0,0,0)}          // 8
};
// address : 0x09 0x0a
//////////////////////////////////////////////////////////////////////
static const rom int vfd_tvmode[3][2] = {
		{DIGIT_L(1,0,1,0,1,1,1,1), DIGIT_H(1,1,1,1,1,0,0,0)},		//480
		{DIGIT_L(1,0,1,0,0,1,1,0), DIGIT_H(0,0,1,1,1,1,0,0)},		//720	
		{DIGIT_L(1,1,1,1,1,1,1,0), DIGIT_H(1,1,1,1,1,1,1,1)}		//1080
};
//////////////////////////////////////////////////////////////////
static const rom int vfd_vgaip[4][2]={
	        {DIGIT_L(1,0,1,0,1,0,0,0), DIGIT_H(1,0,1,0,1,0,0,0)},  // 0
               {DIGIT_L(0,0,0,0,1,0,1,0),DIGIT_H(1,0,1,0,0,0,0,0)},  // 4   
		 {DIGIT_L(1,0,1,0,1,0,1,0),DIGIT_H(1,0,1,0,1,0,0,0)},  // 8
		 {DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)}    // none
};
static const rom int vfd_vgamode[6][2]={
	        {DIGIT_L(1,0,1,0,1,1,1,1),DIGIT_H(1,1,1,1,1,0,0,0)}, //480
	        {DIGIT_L(1,1,1,1,1,1,0,1),DIGIT_H(1,1,1,0,1,1,0,0)},  //600
	        {DIGIT_L(1,0,1,0,1,1,1,0),DIGIT_H(1,0,0,1,1,1,0,0)},   //768
	        {DIGIT_L(1,1,1,1,0,1,1,0),DIGIT_H(0,1,1,1,1,1,0,1)},    //1024
	        {DIGIT_L(1,1,1,1,1,0,0,1),DIGIT_H(1,0,1,1,1,1,0,1)},    //1200
	        {DIGIT_L(0,1,0,0,1,1,0,1),DIGIT_H(0,1,1,0,1,1,0,0)}     //576
};
////////////////////////////////////////////////////////////////////
// address : 0x0c 0x0d
//////////////////////////////////////////////////////////////////////
static const rom int vfd_hd_green[4][2] = {
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,1,0,1,0,1,0)},		//none
		{DIGIT_L(0,0,0,0,0,0,0,1), DIGIT_H(1,0,1,0,1,0,1,0)},		//photo
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,1,0,1,0,1,1)},		//movie
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,1,1,0,1,0,0,0)}		//music
};

static const rom int vfd_hd_blue[4][2] = {
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,0,1,0,1,1,0)},
		{DIGIT_L(0,0,0,0,0,0,0,1), DIGIT_H(1,0,0,1,0,1,1,0)},		//photo
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,0,1,0,1,1,1)},		//movie
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,1,0,1,0,1,0,0)}		//music
};

static const rom int vfd_hd_s[4][2] = {
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,1,0,0,0,0,0)},		//S1
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,0,0,0,1,0,0)},		//S2
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,0,0,1,0,0,0)},		//S3
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,0,1,0,0,0,0)}		//S4
};

static const rom int vfd_mainmode[4][2] = {
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,1,0)},
		{DIGIT_L(0,0,0,0,0,0,0,1), DIGIT_H(0,0,0,0,0,0,1,0)},		//photo
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,1,1)},		//movie
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,1,0,0,0,0,1,0)}		//music

};


// address : 0x0f 0x10
//////////////////////////////////////////////////////////////////////
static const rom int vfd_pcusb[2][2]= {
	{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,1,1)},			//pc usb
	{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)}			//&0xf7
};

static const rom int vfd_playmode[4][2] = {
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,1,1,0)},		//play
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,1,0,1,0)},		//pause
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,1,0,0,1,0)},		//stop
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,1,0)}		//nomode
};

static const rom int vfd_repeat[2][2]= {
	{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,1,0,0,0,1,0)},			//pc usb
	{DIGIT_L(1,1,1,1,1,1,1,1), DIGIT_H(1,1,0,1,1,1,1,1)}			//&0xf7
};

static const rom int vfd_link[4][2] = {
		{DIGIT_L(0,0,0,0,0,0,1,0), DIGIT_H(0,0,0,0,0,0,1,0)},		//hd
		{DIGIT_L(0,0,0,0,0,0,0,1), DIGIT_H(0,0,0,0,0,0,1,0)},		//usb1
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,1,0,0,0,0,1,0)},		//usb2
		{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(1,0,0,0,0,0,1,0)}		//network
};
//////////////////////////////////////////////////////////////////////


void set_vfd_playstatus(int status)
{
	vfd_playstatus = status;
}


void set_vfd_linkstatus(int status)
{
	vfd_linkstatus = status;
}

void set_vfd_pcusbstatus(int status)
{
	vfd_pcusbstatus = status;
}

void set_vfd_repeatstatus(int status)
{
	vfd_repeatstatus = status;
}

void set_vfd_mainstatus(int status)
{
	vfd_mainstatus = status;
}

void vfd_delay_ms(unsigned long time)
{
    //Delay10TCYx(time/50);
	//unsigned long i;
	//for(i=0;i<time*10;i++);
}


void PD6311_WRITE(int reg_data)
{
	int i;
	
	for ( i = 0; i <8; i++ ) {
		
		if((reg_data >> i) & 0x01)
			  PD6311_DIN=1;
		else 
			PD6311_DIN=0;

		PD6311_CLK=1;
		vfd_delay_ms(100);
		PD6311_CLK=0;
		vfd_delay_ms(100);
	}
		

	
}

void VFD_DisplayMode(int mode)
{

   PD6311_STB_TRIS=0;
   PD6311_DIN_TRIS=0;
   PD6311_CLK_TRIS=0;
   vfd_delay_ms(200);
   PD6311_STB=0;
   PD6311_WRITE(0x44);
   PD6311_STB=1;

/////////////////////////////////////////////////////////
   switch(mode)
   	{
   	case VFD_DISPLAY_PC:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);
		   
		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[0][1]|vfd_playmode[vfd_playstatus][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[vfd_linkstatus][1]);	
		   else
		   	PD6311_WRITE(vfd_pcusb[0][1]|(vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[vfd_linkstatus][1]);	
		   
		   PD6311_STB=1;
		   vfd_pcusbstatus= 0;
		   break;
	case VFD_DISPLAY_PCOUT:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);
		   
		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE((vfd_pcusb[1][1]&vfd_playmode[vfd_playstatus][1])|vfd_repeat[vfd_repeatstatus][1]|vfd_link[vfd_linkstatus][1]);		
		   else
		   	PD6311_WRITE((vfd_pcusb[1][1]&vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[vfd_linkstatus][1]);		

		   PD6311_STB=1;
		   vfd_pcusbstatus= 1;
		   break;
	case VFD_DISPLAY_HD:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);
		   
		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[vfd_playstatus][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[0][1]);		
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[0][1]);		

		   PD6311_STB=1;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_link[0][0]);		
		   PD6311_STB=1;
		   vfd_linkstatus = 0;
		   break;
	case VFD_DISPLAY_USB1:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);

		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[vfd_playstatus][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[1][1]);		
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[1][1]);		


		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_link[1][0]);		
		   PD6311_STB=1;
		   vfd_linkstatus = 1;
		   break;
	case VFD_DISPLAY_USB2:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);

		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[vfd_playstatus][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[2][1]);		
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[2][1]);	

		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_link[2][0]);		
		   PD6311_STB=1;
		    vfd_linkstatus = 2;
		   break;
	case VFD_DISPLAY_NETWORK:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);

		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[vfd_playstatus][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[3][1]);
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[3][1]);

		   PD6311_STB=1;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_link[3][0]);		
		   PD6311_STB=1;
		    vfd_linkstatus = 3;
		   break;		   
   	case VFD_DISPLAY_REPEAT:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[vfd_playstatus][1]|vfd_repeat[0][1]|vfd_link[vfd_linkstatus][1]);		
		   PD6311_STB=1;
		   vfd_repeatstatus = 0;
		   break;
	case VFD_DISPLAY_REPEATOFF:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[vfd_playstatus][1]&vfd_repeat[1][1])|vfd_link[vfd_linkstatus][1]);		
		   PD6311_STB=1;
		   vfd_repeatstatus = 1;
		   break;		
   	case VFD_DISPLAY_PLAY:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   
		   vfd_delay_ms(100);
		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[0][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[vfd_linkstatus][1]);		
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[0][1]&vfd_repeat[1][1])|vfd_link[vfd_linkstatus][1]);		

		   PD6311_STB=1;
		   vfd_playstatus = 0;
		   break;
   	case VFD_DISPLAY_PAUSE:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);

		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[1][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[vfd_linkstatus][1]);		
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[1][1]&vfd_repeat[1][1])|vfd_link[vfd_linkstatus][1]);		

		   PD6311_STB=1;
		   vfd_playstatus = 1;
		   break;
   	case VFD_DISPLAY_STOP:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS6_H|0xc0);
		   vfd_delay_ms(100);
		   if(vfd_repeatstatus == 0)
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|vfd_playmode[2][1]|vfd_repeat[vfd_repeatstatus][1]|vfd_link[vfd_linkstatus][1]);		
		   else
		   	PD6311_WRITE(vfd_pcusb[vfd_pcusbstatus][1]|(vfd_playmode[2][1]&vfd_repeat[1][1])|vfd_link[vfd_linkstatus][1]);		

		   PD6311_STB=1;
		   vfd_playstatus = 2;
		   break;	
///////////////////////////////////////////////////////////////////
	case VFD_DISPLAY_NOMODE:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[0][1]);		
		   PD6311_STB=1;
		   vfd_playstatus = 3;
		   break;	



	case VFD_DISPLAY_PHOTO:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[1][1]);		
		   PD6311_STB=1;
		   vfd_mainstatus =1;
		   break;	


	case VFD_DISPLAY_MOVIE:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[2][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[2][1]);		
		   PD6311_STB=1;
		   vfd_mainstatus = 2;
		   break;		

	case VFD_DISPLAY_MUSIC:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[3][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[3][1]);		
		   PD6311_STB=1;
		   vfd_mainstatus = 3;
		   break;	

	case VFD_DISPLAY_ROUNDG:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[vfd_mainstatus][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_green[vfd_mainstatus][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_ROUNDB:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_blue[vfd_mainstatus][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_blue[vfd_mainstatus][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_ROUNDS1:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[0][0]|vfd_mainmode[vfd_mainstatus][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[0][1]|vfd_mainmode[vfd_mainstatus][1]);		
		   PD6311_STB=1;
		   break;	
		   
	case VFD_DISPLAY_ROUNDS2:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[1][0]|vfd_mainmode[vfd_mainstatus][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[1][1]|vfd_mainmode[vfd_mainstatus][1]);		
		   PD6311_STB=1;
		   break;	
	case VFD_DISPLAY_ROUNDS3:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[2][0]|vfd_mainmode[vfd_mainstatus][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[2][1]|vfd_mainmode[vfd_mainstatus][1]);		
		   PD6311_STB=1;
		   break;			   
	case VFD_DISPLAY_ROUNDS4:
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[3][0]|vfd_mainmode[vfd_mainstatus][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS5_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_hd_s[3][1]|vfd_mainmode[vfd_mainstatus][1]);		
		   PD6311_STB=1;
		   break;			   
//////////////////////////////////////////////////////////////
	case VFD_DISPLAY_480I:
		   tv_mode = 0;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[0][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[0][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_480P:
		   tv_mode = 1;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[0][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[1][1]);		
		   PD6311_STB=1;
		   break;	


	case VFD_DISPLAY_720I:
		  tv_mode= 0;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[1][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[0][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_720P:
		  tv_mode = 1;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[1][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[1][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_1080I:
		 tv_mode = 0;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[2][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[2][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[0][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_1080P:
		tv_mode =1;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[2][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvmode[2][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[1][1]);		
		   PD6311_STB=1;
		   break;	
		   
	case VFD_DISPLAY_VGA_480:
		 tv_mode =4;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[0][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[0][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_VGA_600:
		  tv_mode =4;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[1][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[0][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_VGA_768:
		tv_mode =6;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[2][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[2][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[2][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[2][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_VGA_1024:
		tv_mode =5;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[3][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[3][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[1][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[1][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_VGA_1200:
		tv_mode =4;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[4][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[4][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[0][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgaip[0][1]);		
		   PD6311_STB=1;
		   break;	

	case VFD_DISPLAY_576I:
		tv_mode =3;
		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[5][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS4_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_vgamode[5][1]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_L|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[3][0]);		
		   PD6311_STB=1;

		   vfd_delay_ms(100);
		   PD6311_STB=0;
		   PD6311_WRITE(DIGITS3_H|0xc0);
		   vfd_delay_ms(100);
		   PD6311_WRITE(vfd_tvip[3][1]);		
		   PD6311_STB=1;
		   break;	
		   
   	}
/////////////////////////////////////////////////////////		
   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x0b);
   PD6311_STB=1;
   

   
   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x8e);
   PD6311_STB=1;
}


void VFD_DisplayTime(unsigned char hour,unsigned char min,unsigned char sec)
{
	
   int min_high, min_low, sec_high, sec_low;
   int tvmode = tv_mode;
   min_high = min/10;
   min_low = min%10;
   sec_high = sec/10;
   sec_low = sec%10;

   PD6311_STB_TRIS=0;
   PD6311_DIN_TRIS=0;
   PD6311_CLK_TRIS=0;
   vfd_delay_ms(200);
   PD6311_STB=0;
   PD6311_WRITE(0x44);
   PD6311_STB=1;

/////////////////////////////////////////////////////////
   
   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x00|0xc0);
   vfd_delay_ms(100);
   PD6311_WRITE((vfd_hour[hour][0]|vfd_min_high[min_high][0]));		
   PD6311_STB=1;

   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x01|0xc0);
   vfd_delay_ms(100);
   PD6311_WRITE((vfd_hour[hour][1])|(vfd_min_high[min_high][1]));		
   PD6311_STB=1;

   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x03|0xc0);
   vfd_delay_ms(100);
   PD6311_WRITE((vfd_min_low[min_low][0])|(vfd_sec_high[sec_high][0]));	
   PD6311_STB=1;


   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x04|0xc0);
   vfd_delay_ms(100);
   PD6311_WRITE((vfd_min_low[min_low][1]|vfd_sec_high[sec_high][1]));		
   PD6311_STB=1;


   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x06|0xc0);
   vfd_delay_ms(100);
   PD6311_WRITE((vfd_sec_low[sec_low][0]|vfd_tvip[tvmode][0]));		
   PD6311_STB=1;

   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x07|0xc0);
   vfd_delay_ms(100);
    PD6311_WRITE((vfd_sec_low[sec_low][1]|vfd_tvip[tvmode][1]));			
   PD6311_STB=1;

/////////////////////////////////////////////////////////		
   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x0b);
   PD6311_STB=1;
   

   
   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x8e);
   PD6311_STB=1;
}


void PD6311_Init(void)
{
	int i=0;

   PD6311_STB_TRIS=0;
   PD6311_DIN_TRIS=0;
   PD6311_CLK_TRIS=0;
   vfd_delay_ms(200);
   PD6311_STB=0;
   PD6311_WRITE(0x40);
   PD6311_STB=1;

   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0xc0);
   
   vfd_delay_ms(100);
   for(i=0;i<48;i++)
   	{
   vfd_delay_ms(100);
   PD6311_WRITE(0x00);
   
   	}
   PD6311_STB=1;


   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x0b);
   PD6311_STB=1;
   

   
   vfd_delay_ms(100);
   PD6311_STB=0;
   PD6311_WRITE(0x8e);
   PD6311_STB=1;
}

void PD6311_SetSW(void)
{
	//int i=0;

  PD6311_STB_TRIS=0;
  PD6311_DIN_TRIS=0;
  PD6311_CLK_TRIS=0;
  
   vfd_delay_ms(200);
   PD6311_STB=0;
   PD6311_WRITE(0x41);		//commod 2
   vfd_delay_ms(100);
   if(vfd_ON_OFF == 0)
   {
      PD6311_WRITE(0x00);
      vfd_ON_OFF = 1;
   }
   else
   {
      PD6311_WRITE(0x01);
      vfd_ON_OFF = 0;	  
   }
   PD6311_STB=1;

}

void PD6311_OFF(unsigned int On_Off)
{
	//int i=0;

  PD6311_STB_TRIS=0;
  PD6311_DIN_TRIS=0;
  PD6311_CLK_TRIS=0;
  
   vfd_delay_ms(200);
   PD6311_STB=0;
   PD6311_WRITE(0x41);		//commod 2
   vfd_delay_ms(100);
    if(On_Off== 1)   
       PD6311_WRITE(0x00);       
   else  
       PD6311_WRITE(0x01);      
  
   PD6311_STB=1;

}

void Check_Playing(void)
{
   #if 0
    em86xx_gpio_write(3,1);
   #endif
}

void Check_Stop(void)
{
	#if 0
    em86xx_gpio_write(3,0);
	#endif
}