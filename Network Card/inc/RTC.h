/*
 * RTC.h
 *
 *  Created on: Jan. 6 2017
 *      Author: Mitchell Larson
 */

#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include "date.h"
#include <string.h>
#include <stdio.h>

#define INIT 7
#define INITF 6
#define SYNCHPREDIV 255
#define ASYNCHPREDIV 127
#define PM 22
#define FMT 6
#define TIME_LENGTH 11

typedef struct{
	uint32_t TR;
	uint32_t DR;
	uint32_t CR;
	uint32_t ISR;
	uint32_t PRER;
	uint32_t WUTR;
	uint32_t CALIBR;
	uint32_t ALRMAR;
	uint32_t ALRMBR;
	uint32_t WPR;
	uint32_t SSR;
	uint32_t SHIFTR;
	uint32_t TSTR;
	uint32_t TSDR;
	uint32_t TSSSR;
	uint32_t CALR;
	uint32_t TAFCR;
	uint32_t ALRMASSR;
	uint32_t ALRMBSSR;
	//backup registers
}RTC_Struct;

typedef enum {HOUR24, HOUR12} H12;

typedef struct{
	uint8_t RTC_Hours;
	uint8_t RTC_Minutes;
	uint8_t RTC_Seconds;
	H12 timeMode;
} RTC_Time;

typedef struct{
	WeekDay RTC_WeekDay;
	Month RTC_Month;
	uint8_t RTC_Date; 		//1-31
	uint8_t RTC_Year;		//0-99
} RTC_Date;

extern void init_rtc(RTC_Date*,RTC_Time*);
extern void setDate(RTC_Date*);
extern void setTime(RTC_Time*);
extern char* time_to_string(char* time);
extern uint8_t get_Hour();

#endif
