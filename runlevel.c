#include "DevelOS.h"

// <editor-fold defaultstate="collapsed" desc="RL_Standby      0x01">
void DoStandby(void)
{
    // Just show uptime and clock. 
    // TODO: Try to reduce the core clock in this mode to save energy
    // But beware: so far this has always messed up everything

    char i;
     
    for(i=6;i<14;i++)
    {
        console.Buffer[1][i]=rtc.string[i-6];
    }
    c_pos(6,2);
//    sysprint(0, sysstr_lps, 1);
//    c_value(OS.LPS);
//    c_pos(6,3);
//    c_value(OS.CPUIdlePercent);
//    sysprint(0, sysstr_idle, 2);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_Slideshow    0x02">
void DoSlideshow(void)
{
#ifdef MOD_Display_VFL
    switch(OS.submode)
    {
        case 0:
            VFLD_clr();
            VFLD.Duty=0;
            VFLD_scroll("+ 3,3     \n",2);
            OS.submode++;
            break;
        case 1:
            if(VFLD.ScrollSpeed == 0)
            {
                VFLD.Symbol[VFLD_CN2]=1;
                VFLD.Symbol[VFLD_DEC]=1;
                VFLD.Duty=7;
                VFLD_Value(OS.U3V3/10);
                VFLD_Refresh();
                OS.submode++;
            }
            break;
        case 2:
            OS_delay_1S();
            OS_delay_1S();
            OS.submode++;
            break;
        case 3:
             VFLD_clr();
             VFLD.Duty=0;
             VFLD_scroll("+ 5,0     \n",2);
             OS.submode++;
             break;
       case 4:
            if(VFLD.ScrollSpeed == 0)
            {
                VFLD.Symbol[VFLD_CN2]=1;
                VFLD.Symbol[VFLD_DEC]=1;
                VFLD.Duty=7;
                VFLD_Value(OS.U5V0/10);
                VFLD_Refresh();
                OS.submode++;
            }
            break;
        case 5:
            OS_delay_1S();
            OS_delay_1S();
            OS.submode++;
            break;
        case 6:
            VFLD_clr();
            VFLD.Duty=0;
            VFLD_scroll("+12,0     \n",2);
            OS.submode++;
            break;
        case 7:
            if(VFLD.ScrollSpeed == 0)
            {
                VFLD.Symbol[VFLD_CN2]=1;
                VFLD.Symbol[VFLD_DEC]=1;
                VFLD.Duty=7;
                VFLD_Value(OS.U12V/10);
                VFLD_Refresh();
                OS.submode++;
            }
            break;
        case 8:
            OS_delay_1S();
            OS_delay_1S();
            OS.submode++;
            break;
        case 9:
            VFLD_clr();
            VFLD.Duty=0;
            VFLD_scroll("CPU MHZ   \n",2);
            OS.submode++;
            break;
        case 10:
            if(VFLD.ScrollSpeed == 0)
            {
                VFLD.Symbol[VFLD_CN1]=1;
                VFLD.Duty=7;
                VFLD_Value(OS.CPUClock/1000);
                VFLD_Refresh();
                OS.submode++;
            }
            break;
        case 11:
            OS_delay_1S();
            OS_delay_1S();
            OS.submode=0;
            break;
    }
#endif
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_3v3          0x03">
void Show3v3(void)
{
//    d_print("+ 3,3     \n");
//    //VFLD.Symbol[VFLD_CN2]=1;
//    //VFLD.Symbol[VFLD_DEC]=1;
//    d_value(OS.U3V3/10);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_5v           0x04">
void Show5v(void)
{
//    d_print("+ 5,0     \n");
//    //VFLD.Symbol[VFLD_CN2]=1;
//    //VFLD.Symbol[VFLD_DEC]=1;
//    d_value(OS.U5V0/10);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_12v          0x05">
void Show12v(void)
{
//    d_print("+12,0     \n");
//    //VFLD.Symbol[VFLD_CN2]=1;
//    //VFLD.Symbol[VFLD_DEC]=1;
//    d_value(OS.U12V/10);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_PWM_H        0x10">
#if defined MOD_pwm
void DoHF_PWM(void)
{    
    OS.PWM_Value[0]=OS.HF_duty0;
    OS.PWM_Value[1]=OS.HF_period0;
    OS.PWM_Value[2]=OS.HF_duty1;
    OS.PWM_Value[3]=OS.HF_period1;

    switch(OS.runmode)
    {
        case 0:                                                                 // Entry Point
            VFLD_clr();
            VFLD_scroll("  HF PWM  \n",2);
            OS.runmode++;
            break;
        case 1:
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_ns(100000000);
                VFLD_scroll( PWMstring[ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 2:                                                                 // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L] == -1 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_clr();
                    VFLD_print( PWMstring[ OS.submode ]);
                }
                if(KP.Key[KP_R] == -1 & OS.submode<5)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_clr();
                    VFLD_print( PWMstring[ OS.submode ]);
                }
                if(OS.submode<4)
                {
                    VFLD_Value(OS.PWM_Value[OS.submode]);
                    if(KP.Key[KP_GN] == -1)
                    {
                         KP.Key[KP_GN]=0;
                         //OS.runmode=10;       // enter number
                    }
                    if(KP.Key[KP_C] == -1)
                    {
                        OS_delay_ns(50000000);
                         KP.Key[KP_C]=0;
                         OS.runmode=5;              // inc/dec value
                    }
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;

        case 5:                                                                 // Inkrementieren / Dekrementieren
            if(VFLD.ScrollSpeed==0)
            {
                switch(OS.submode)
                {
                    case 0:
                        if(KP.Key[KP_L] == -1 & OS.PWM_Value[OS.submode] > 0 )
                        {
                            KP.Key[KP_L]=0;
                            KP.Key[KP_R]=0;
                            OS.PWM_Value[OS.submode]--;
                        }
                        if(KP.Key[KP_R] == -1 & OS.PWM_Value[OS.submode] < OS.HF_period0 )
                        {
                            KP.Key[KP_R]=0;
                            KP.Key[KP_L]=0;
                            OS.PWM_Value[OS.submode]++;
                        }
                        if(KP.Key[KP_C] == -1)
                        {
                             KP.Key[KP_C]=0;
                             OS.runmode=1;
                        }
                        break;
                    case 1:
                        if(KP.Key[KP_L] == -1 & OS.PWM_Value[OS.submode] > 0 )
                        {
                            OS_delay_ns(50000000);
                            KP.Key[KP_L]=0;
                            KP.Key[KP_R]=0;
                            OS.PWM_Value[OS.submode]--;
                        }
                        if(KP.Key[KP_R] == -1 & OS.PWM_Value[OS.submode] < 9999 )
                        {
                            OS_delay_ns(50000000);
                            KP.Key[KP_R]=0;
                            KP.Key[KP_L]=0;
                            OS.PWM_Value[OS.submode]++;
                        }
                        if(KP.Key[KP_C] == -1)
                        {
                            OS_delay_ns(50000000);
                             KP.Key[KP_C]=0;
                             OS.runmode=1;
                        }
                        break;
                    case 2:
                        if(KP.Key[KP_L] == -1 & OS.PWM_Value[OS.submode] > 0 )
                        {
                            KP.Key[KP_L]=0;
                            KP.Key[KP_R]=0;
                            OS.PWM_Value[OS.submode]--;
                        }
                        if(KP.Key[KP_R] == -1 & OS.PWM_Value[OS.submode] < OS.HF_period1 )
                        {
                            KP.Key[KP_R]=0;
                            KP.Key[KP_L]=0;
                            OS.PWM_Value[OS.submode]++;
                        }
                        if(KP.Key[KP_C] == -1)
                        {
                             KP.Key[KP_C]=0;
                             OS.runmode=1;
                        }
                        break;
                    case 3:
                        if(KP.Key[KP_L] == -1 & OS.PWM_Value[OS.submode] > 0 )
                        {
                            KP.Key[KP_L]=0;
                            KP.Key[KP_R]=0;
                            OS.PWM_Value[OS.submode]--;
                        }
                        if(KP.Key[KP_R] == -1 & OS.PWM_Value[OS.submode] < 9999 )
                        {
                            KP.Key[KP_R]=0;
                            KP.Key[KP_L]=0;
                            OS.PWM_Value[OS.submode]++;
                        }
                        if(KP.Key[KP_C] == -1)
                        {
                             KP.Key[KP_C]=0;
                             OS.runmode=1;
                        }
                        break;
                    default:
                        break;
                }
                VFLD_Value(OS.PWM_Value[OS.submode]);
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        
        default:
            OS_SetRunlevel(RL_SetRL);
            break;
    }

    OS.HF_duty0     =OS.PWM_Value[0];
    OS.HF_period0   =OS.PWM_Value[1];
    OS.HF_duty1     =OS.PWM_Value[2];
    OS.HF_period1   =OS.PWM_Value[3];

}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_SetKeypad    0xE0">
#ifdef MOD_keypad
void KeypadSetup(void)
{
    char l;
    VFLD_clr();
    if(OS.runmode==0)
    {
        VFLD_scroll("SET KEYPAD\n",2);
        OS.runmode++;
    }
    else if(OS.runmode<=17)
    {
        if(VFLD.ScrollSpeed==0)
        {
            VFLD_print("PRESS     \n");
            VFLD_Value(OS.runmode-1);                                         // print keycode to press
            for(l=0;l<Scanlines;l++)
            {
                if(KP.Scanline[l]>20)                                       // TODO: use bias-value from variable
                {
                    SetKeylimit(OS.runmode-1, l, KP.Scanline[l], 10 );         // store result
                    OS.runmode++;                                           // repeat for each key
                    OS_delay_ns(50000000);
                }   // key pressed
            }
        }
    }
    else if( OS.runmode == 18)
    {
        VFLD_scroll("WRITE  ROM\n",2);
        OS.runmode++;
    }
    else
    {
        if(VFLD.ScrollSpeed==0)
        {
            OS_delay_ns(50000000);
            SaveEEPROM_KP(1);
            OS.runmode=0;
            OS.runlevel=RL_TestKeypad; //OS.prev_runlevel;                                 // Done
            OS.prev_runlevel=RL_SetKeypad;
        }
    }
}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_TestKeypad   0xE1">
#ifdef MOD_keypad
void KeypadTest(void)
{
    char i;

    VFLD_clr();
    VFLD_print("TASTE     \n");
    VFLD.Symbol[VFLD_CN2]=1;
    for(i=1;i<17;i++)
    {
        if(KP.Key[i]>0)
        {
            VFLD_Value(i);
            VFLD_Refresh();
            KP.Key[i]=0;
        }
    }
}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_TuneKeypad   0xE2">
#ifdef MOD_keypad
void DoKeyTune(void)
{
    if(OS.runmode<16)
    {
        KP.Key[KP_C]=0;
        VFLD_print("KEY 00    \n");
        VFLD.Symbol[VFLD_RO]=0;
        if(OS.runmode<10)
        {
            VFLD.String[5]=OS.runmode;
        }
        else
        {
            VFLD.String[5]=OS.runmode+55;
        }
        VFLD.Symbol[VFLD_CN2]=1;

        if( KP.Scanline[ KP.Matrix[OS.runmode].line ] > 20 )
        {
            VFLD_Value( KP.Scanline[ KP.Matrix[OS.runmode].line ] );

            if( ( KP.Scanline[ KP.Matrix[OS.runmode].line ] > KP.Matrix[OS.runmode].limit1 ) & ( KP.Scanline[ KP.Matrix[OS.runmode].line ] > KP.Matrix[OS.runmode].limit0 ) )
            {
                KP.Matrix[OS.runmode].limit1++;
                VFLD.Symbol[VFLD_RO]=1;
            }
            if( ( KP.Scanline[ KP.Matrix[OS.runmode].line ] < KP.Matrix[OS.runmode].limit0 ) & ( KP.Scanline[ KP.Matrix[OS.runmode].line ] < KP.Matrix[OS.runmode].limit1 ))
            {
                KP.Matrix[OS.runmode].limit0--;
                VFLD.Symbol[VFLD_RO]=1;
            }
            VFLD_Refresh();
        }
        ScanKeys();
        if(KP.Key[KP_C]>0)
        {
            KP.Key[KP_C]=0;
            OS.runmode++;
            OS_delay_ns(50000000);
        }
        KP.Key[0]=0;

    }
    else
    {
        OS_SetRunlevel(RL_Setup);
    }
}
#endif
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_ROM_Stat     0xE3">
void DoROMstat(void)
{
    switch(OS.runmode)
    {
        case 0:
            //VFLD_scroll("BLOCKS    \n",2);
            OS.runmode++;
            break;
        case 1:
//            if(VFLD.ScrollSpeed==0)
  //          {
                //VFLD.Symbol[VFLD_CN2]=1;
                //VFLD.String[7]=8-eprom.FreeBlocks;
                //VFLD.String[8]='/';
                //VFLD.String[9]=8;
                OS.runmode++;
    //        }
            break;
        case 2:
//            if( KP.Key[KP_L]>0 | KP.Key[KP_R]>0 )
            {
                OS.runmode++;
            }
            break;
        default:
            OS_SetRunlevel(RL_Setup);
            break;

    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_Setup        0xEF">
void OS_SetupMenu(void)
{
    #ifdef MOD_keypad
    switch(OS.runmode)
    {
        case 0:                                                                 // Entry Point
            VFLD_scroll("  SETUP   \n",2);
            OS.runmode++;
            break;
        case 1:                                                                 // Setup Menu
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_1S();
                VFLD_scroll( setupstring[0][ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 2:                                                                 // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L]>0 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_scroll( setupstring[0][ OS.submode ], -2);
                }
                if(KP.Key[KP_R]>0 & OS.submode<5)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_scroll( setupstring[0][ OS.submode ], 2);
                }
                if(KP.Key[KP_C]>0)
                {
                     KP.Key[KP_C]=0;
                     OS.runmode++;
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        case 3:                                                                 // Main Choice
            switch(OS.submode)
            {
                case 4:                                                         // Reset Chip
                    Reset();
                    break;
                case 5:                                                         // Exit setup
                    OS.runmode=0xFF;
                    break;
                default:                                                        // Submenu
                    OS.runmode=20 + (OS.submode * 40);
                    OS.submode=0;
                    break;
            }
            break;


        case 20:                                                                // Keypad Menu
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_1S();
                VFLD_scroll( setupstring[1][ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 21:                                                                // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L]>0 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_scroll( setupstring[1][ OS.submode ], -2);
                }
                if(KP.Key[KP_R]>0 & OS.submode<3)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_scroll( setupstring[1][ OS.submode ], 2);
                }
                if(KP.Key[KP_C]>0)
                {
                     KP.Key[KP_C]=0;
                     OS.runmode++;
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        case 22:                                                                // Keypad Choice
            switch(OS.submode)
            {
                case 0:                                                         // Calibration
                    OS_SetRunlevel(RL_SetKeypad);
                    break;
                case 1:                                                         // Test
                    OS_SetRunlevel(RL_TestKeypad);
                    break;
                case 2:                                                         // Tuning
                    OS_SetRunlevel(RL_TuneKeypad);
                case 3:                                                         // Back
                    OS.submode=0;
                    OS.runmode=0;
                    break;
                default:
                    OS.runmode=0xFF;
                    break;
            }
            break;


        case 60:                                                                // ROM Menu
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_1S();
                VFLD_scroll( setupstring[2][ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 61:                                                                // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L]>0 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_scroll( setupstring[2][ OS.submode ], -2);
                }
                if(KP.Key[KP_R]>0 & OS.submode<5)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_scroll( setupstring[2][ OS.submode ], 2);
                }
                if(KP.Key[KP_C]>0)
                {
                     KP.Key[KP_C]=0;
                     OS.runmode++;
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        case 62:                                                                // ROM Choice
            switch(OS.submode)
            {
                case 0:                                                         // status
                    OS_SetRunlevel(RL_ROM_Stat);
                    break;
                case 1:                                                         // Format
                    EE_format();
                    OS.submode=0;
                    OS.runmode=60;
                    VFLD_clr();
                    break;
                case 2:
                    OS_SetRunlevel(RL_ROM_Backup);                              //Backup
                    break;
                case 3:                                                         // Write KP
                    SaveEEPROM_KP(1);
                    OS.submode=5;
                    break;
                case 4:                                                         // Write OS
                    SaveEEPROM_OS(0);
                    OS.submode=5;
                    break;

                case 5:                                                         // Back
                    OS.submode=0;
                    OS.runmode=0;
                    break;
                default:
                    OS.runmode=0xFF;
                    break;
            }
            break;

        case 100:                                                               // Clock Menu
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_1S();
                VFLD_scroll( setupstring[3][ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 101:                                                               // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L]>0 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_scroll( setupstring[3][ OS.submode ], -2);
                }
                if(KP.Key[KP_R]>0 & OS.submode<4)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_scroll( setupstring[3][ OS.submode ], 2);
                }
                if(KP.Key[KP_C]>0)
                {
                     KP.Key[KP_C]=0;
                     OS.runmode++;
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        case 102:                                                               // Clock Choice
            switch(OS.submode)
            {
                case 0:                                                         // Set RTC
                    OS_SetRunlevel(RL_CLK_RTC);
                    break;
                case 1:
                    OS_SetRunlevel(RL_CLK_CPU);
                    break;
                case 2:
                    OS_SetRunlevel(RL_CLK_Tune);
                    break;
                case 3:
                    OS_SetRunlevel(RL_CLK_SYS);
                    break;
                case 4:                                                         // Back
                    OS.submode=0;
                    OS.runmode=0;
                    break;
                default:
                    OS.runmode=0xFF;
                    break;
            }
            break;

        case 140:                                                               // Sys Menu
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_1S();
                VFLD_scroll( setupstring[4][ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 141:                                                               // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L]>0 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_scroll( setupstring[4][ OS.submode ], -2);
                }
                if(KP.Key[KP_R]>0 & OS.submode<5)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_scroll( setupstring[4][ OS.submode ], 2);
                }
                if(KP.Key[KP_C]>0)
                {
                     KP.Key[KP_C]=0;
                     OS.runmode++;
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        case 142:                                                               // Sys Choice
            switch(OS.submode)
            {
                case 0:
                    OS_SetRunlevel(RL_SYS_Perf);
                    break;
                case 1:
                    OS_SetRunlevel(RL_SYS_Reset);
                    break;
                case 2:
                    OS_SetRunlevel(RL_SYS_Uptime);
                    break;
                case 3:
                    OS_SetRunlevel(RL_SYS_Alarm);
                    break;
                case 4:
                    InitOS();
                    OS_SetRunlevel(RL_Boot);
                    break;
                case 5:                                                         // Back
                    OS.submode=0;
                    OS.runmode=0;
                    break;
                default:
                    OS.runmode=0xFF;
                    break;
            }
            break;

        case 0xFF:      // Exit
            OS_SetRunlevel(RL_SetRL);
            break;
        default:
            OS_SetRunlevel(RL_Standby);
            break;
    }
    #endif

}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_SetRL        0xF0">
void OS_SetRL(void)
{
    int i;
#ifdef MOD_keypad
    switch(OS.runmode)
    {
        case 0:
            VFLD_clr();
            VFLD_scroll("+SET MODE+\n",2);
            for(i=0;i<16;i++)
            {
                KP.Key[i]=0;
            }
            OS.runmode++;
            break;
        case 1:
            if(VFLD.ScrollSpeed==0)
            {
                OS_delay_ns(100000000);
                VFLD_scroll( modestring[ OS.submode ], 2);
                OS.runmode++;
            }
            break;
        case 2:                                                                 // Show Menuitem
            if(VFLD.ScrollSpeed==0)
            {
                if(KP.Key[KP_L]>0 & OS.submode>0)
                {
                    KP.Key[KP_L]=0;
                    KP.Key[KP_R]=0;
                    OS.submode--;
                    VFLD_scroll( modestring[ OS.submode ], -2);
                }
                if(KP.Key[KP_R]>0 & OS.submode<7)
                {
                    KP.Key[KP_R]=0;
                    KP.Key[KP_L]=0;
                    OS.submode++;
                    VFLD_scroll( modestring[ OS.submode ], 2);
                }
                if(KP.Key[KP_C]>0)
                {
                     KP.Key[KP_C]=0;
                     OS.runmode++;
                }
            }
            else
            {
                KP.Key[KP_L]=0;
                KP.Key[KP_R]=0;
                KP.Key[KP_C]=0;
            }
            break;
        case 3:                                                                 // Main Choice
            switch(OS.submode)
            {
                case 0:
                    OS_SetRunlevel(RL_3v3);
                    break;
                case 1:
                    OS_SetRunlevel(RL_5v);
                    break;
                case 2:
                    OS_SetRunlevel(RL_12v);
                    break;
                case 3:
                    OS_SetRunlevel(RL_PWM_H);
                    break;
                case 4:
                    OS_SetRunlevel(RL_PWM_L);
                    break;
                case 5:
                    OS_SetRunlevel(RL_Setup);
                    break;
                case 6:
                    OS_SetRunlevel(RL_Slideshow);
                    break;
                case 7:
                    OS_SetRunlevel(RL_Standby);
                    break;

                default:                                                        // Submenu
                    OS_SetRunlevel(RL_Standby);
                    break;
            }
            break;

    }
#endif
    
    if(OS.runmode==0)
    {
    }
    else
    {
    }
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_Boot         0xFE">
void BootLevel(void){
    //d_clr();
    //d_print("  READY   \n");
    RefreshDisplay();
    OS_delay_1S();
    OS_SetRunlevel(Startmode);
}// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="RL_Error        0xFF">
void ErrorLevel(void){
//    d_clr();
//    d_print("ERROR     \n");
//    //VFLD.Symbol[VFLD_CN2]=1;
//    d_value(OS.prev_runlevel);
    RefreshDisplay();
} // </editor-fold>
