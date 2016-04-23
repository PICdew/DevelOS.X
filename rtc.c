/*      DevelOS
 * File:    rtc.c
 * Author:  Steffis PC
 * Destiny: Realtime Clock Source
 *
 * Created on 18. April 2013, 14:42
 */

#include "./DevelOS.h"

#ifdef MOD_rtc

void IncrementRTC(void)     // Call once per sec, takes care of the rest.
{
    rtc.secs++;

    if (rtc.secs == 60)
    {
        rtc.mins++; rtc.secs=0;
        if (rtc.mins == 60)
        {
            rtc.hour++; rtc.mins=0;
            if (rtc.hour == 24)
            {
                rtc.days++; rtc.hour=0;
                if (  (rtc.days == 29 && rtc.mont==2 && !IS_LEAP(rtc.year))                        // February in leap year
                    || (rtc.days == 30 && rtc.mont==2)                                          // February in normal years
                    || (rtc.days == 31 && (rtc.mont==4 || rtc.mont==6 || rtc.mont==9 || rtc.mont==11 ))  // All months with 30 rtc.days
                    || (rtc.days == 32) )                                                     // All months with 31 rtc.days
                {
                    rtc.mont++;
                    rtc.days=1;
                }
                if (rtc.mont == 13)
                {
                    rtc.year++;
                    rtc.mont=1;
                }
            }
        }
    }
}

#endif

