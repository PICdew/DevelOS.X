/*      DevelOS
 * File:    rtc.h
 * Author:  Steffis PC
 * Destiny: Realtime Clock Header
 *
 * Created on 18. April 2013, 14:42
 */

#ifndef RTC_H
#define	RTC_H

#ifdef	__cplusplus
extern "C" {
#endif

extern struct RTC_Time
{
    unsigned char secs;
    unsigned char mins;
    unsigned char hour;
    unsigned char days;
    unsigned char mont;
    unsigned char year;
} rtc;

#define IS_LEAP(year) (year%4 == 0)

void IncrementRTC(void);

#ifdef	__cplusplus
}
#endif

#endif	/* RTC_H */

