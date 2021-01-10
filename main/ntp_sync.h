/*
 *  Copyright 2020-2021 Fredrik Andersson <fredrik.k.andersson@gmail.com>
 *
 *  This work is licensed under the CC BY-NC-SA 4.0 International License.
 * 
 */

#ifndef NTPSYNCH
#define NTPSYNCH

#include <stdbool.h>
#include <time.h>

#include <esp_sntp.h>

/**
 * Return the current unix epoch time
 *
 * Note that ntp_time_init() should be called to set the time,
 * or this method will return 0.
 *
 * @return  Current epoch (number of seconds that have elapsed since
 *          January 1, 1970 (midnight UTC/GMT)) 
 */
int64_t get_current_epoch(void);

/**
 * Sets local time from a NTP server
 * 
 * Note internet access must be enabled for this to work!
 *
 * @param ntp_server A null terminated string containing the address
 *                   of the ntp server
 */
int ntp_time_init(const char* ntp_server);

#endif
