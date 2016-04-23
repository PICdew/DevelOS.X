/*      DevelOS
 * File:    runlevel.h
 * Author:  Steffis PC
 * Destiny: Define all the runlevels
 *
 * Created on 22. April 2013, 18:48
 */

#ifndef RUNLEVEL_H
#define	RUNLEVEL_H

#ifdef	__cplusplus
extern "C" {
#endif

    // Runlevel class 0: Überwachungsfunktionen
#define RL_Standby      0x01    // just show the time, still do all the os stuff in background
#define RL_Slideshow    0x02    // cycle through all the interesting values
#define RL_3v3          0x03    // show voltage on +3,3V
#define RL_5v           0x04    // show voltage on +5V
#define RL_12v          0x05    // show voltage on +12V

    // Runlevel class 1: Signalerzeugung
#define RL_PWM_H        0x10    // HF-PWM 
#define RL_PWM_L        0x11    // LF-PWM timer-controlled (20ms period, 1-2ms duty)

    // Runlevel class E: Setupfunktionen
#define RL_SetKeypad    0xE0    // calibration
#define RL_TestKeypad   0xE1    // Test
#define RL_TuneKeypad   0xE2    // Tuning
#define RL_ROM_Stat     0xE3
#define RL_ROM_Format   0xE4
#define RL_ROM_Backup   0xE5
#define RL_CLK_RTC      0xE6    // Set Time & Date
#define RL_CLK_CPU      0xE7
#define RL_CLK_Tune     0xE8
#define RL_CLK_SYS      0xE9
#define RL_SYS_Perf     0xEA
#define RL_SYS_Reset    0xEB
#define RL_SYS_Uptime   0xEC
#define RL_SYS_Alarm    0xED
#define RL_SYS_Init     0xEE
#define RL_Setup        0xEF    // Setup Menü

    // Runlevel class F: System
#define RL_SetRL        0xF0    // Set new Runlevel
#define RL_Boot         0xFE    // Should be set @ reset
#define RL_Error        0xFF    // something went wrong

// runlevel Functions
void ErrorLevel(void);
void BootLevel(void);
void KeypadSetup(void);
void KeypadTest(void);
void DoKeyTune(void);
void DoROMstat(void);
void Show3v3(void);
void Show5v(void);
void Show12v(void);
void DoHF_PWM(void);

//void OS_SetupMenu(void);
//void OS_SetRL(void);
    // Runlevel Functions
void DoSlideshow(void);

#ifdef	__cplusplus
}
#endif

#endif	/* RUNLEVEL_H */

