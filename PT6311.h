/*****************************************
 *  Copyright (c) 2006
 *  HIVISION, Inc. All Rights Reserved
 *  Modified for KiSS Front Panel support
 *  Proprietary and Confidential
 ******************************************/
                                                                                

#ifndef _PT6311_H_
#define _PT6311_H_

#define PD6311_STB    		PORTEbits.RE1
#define PD6311_STB_TRIS		TRISEbits.TRISE1
#define PD6311_CLK		PORTEbits.RE0
#define PD6311_CLK_TRIS		TRISEbits.TRISE0
#define PD6311_DIN		PORTEbits.RE2
#define PD6311_DIN_TRIS         TRISEbits.TRISE2
#define PD6311_DOUT		PORTEbits.RA0
#define PD6311_DOUT_TRIS        TRISAbits.TRISA0

#define VFD_DISPLAY 	1
#define VFD_CLOSE		0

#define DIGIT_L(v2d,v1d,v2e,v1e,v2c,v1c,v2g,v1g)	((v1g << 7) | (v2g << 6) | (v1c << 5) | (v2c << 4) | (v1e << 3) | (v2e << 2) | (v1d << 1) | v2d)
#define DIGIT_H(v2f,v1f,v2b,v1b,v2a,v1a,vcol,vq)	((vq << 7) | (vcol << 6) | (v1a << 5) | (v2a << 4) | (v1b << 3) | (v2b << 2) | (v1f << 1) | v2f)

#define DIGITS1_L 	0x00
#define DIGITS1_H 	0x01
#define DIGITS2_L 	0x03
#define DIGITS2_H 	0x04
#define DIGITS3_L 	0x06
#define DIGITS3_H 	0x07
#define DIGITS4_L 	0x09
#define DIGITS4_H 	0x0a
#define DIGITS5_L	0x0c
#define DIGITS5_H 	0x0d
#define DIGITS6_L	0x0f
#define DIGITS6_H 	0x10





enum Display
{
	VFD_DISPLAY_PC = 0,
	VFD_DISPLAY_PCOUT ,
	VFD_DISPLAY_POWER,
	VFD_DISPLAY_HD,   //3//3
	VFD_DISPLAY_USB1,
	VFD_DISPLAY_USB2,
	VFD_DISPLAY_NETWORK,
	VFD_DISPLAY_PLAY,
	VFD_DISPLAY_PAUSE,
	VFD_DISPLAY_STOP,
	VFD_DISPLAY_REPEAT,
	VFD_DISPLAY_REPEATOFF,
	VFD_DISPLAY_MOVIE,
	VFD_DISPLAY_MUSIC,
	VFD_DISPLAY_NOMODE,
	VFD_DISPLAY_PHOTO,
	VFD_DISPLAY_480I,
	VFD_DISPLAY_480P,
	VFD_DISPLAY_720I,
	VFD_DISPLAY_720P,
	VFD_DISPLAY_1080I,
	VFD_DISPLAY_1080P,
	VFD_DISPLAY_ROUNDG,
	VFD_DISPLAY_ROUNDB,	
	VFD_DISPLAY_ROUNDS1,
	VFD_DISPLAY_ROUNDS2,
	VFD_DISPLAY_ROUNDS3,
	VFD_DISPLAY_ROUNDS4,
	VFD_DISPLAY_VGA_480,
	VFD_DISPLAY_VGA_600,
	VFD_DISPLAY_VGA_768,
	VFD_DISPLAY_VGA_1024,
	VFD_DISPLAY_VGA_1200,
	VFD_DISPLAY_576I
};

void set_vfd_playstatus(int status);


void set_vfd_linkstatus(int status);


void set_vfd_pcusbstatus(int status);


void set_vfd_repeatstatus(int status);


void set_vfd_mainstatus(int status);



void PD6311_WRITE(int reg_data);
void PD6311_Init(void);
void PD6311_SetSW(void);
void PD6311_OFF(unsigned int On_Off);
void vfd_delay_ms(unsigned long time);
void VFD_DisplayMode(int mode);
void VFD_DisplayTime(unsigned char hour,unsigned char min,unsigned char sec);

void Check_Playing(void);
void Check_Stop(void);
#endif /* _PT6311_H_ */