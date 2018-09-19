#include "RTC.h"
#include "RCC.h"


#define PWR_CR (volatile uint32_t*) 0x40007000

static volatile RCC_Struct* RCC = (RCC_Struct*) 0x40023800;
static volatile RTC_Struct* RTC = (RTC_Struct*) 0x40002800;

void init_rtc(RTC_Date*,RTC_Time*);
void setDate(RTC_Date*);
void setTime(RTC_Time*);
char* time_to_string(char* time);
uint8_t get_Hour();
static void disable_RTC_write_protect();
static void enable_RTC_write_protect();
static void disable_RTC_init();
static void enable_RTC_init();
static uint8_t RTC_ByteToBcd2(uint8_t Value);

/**
 * This function will initialize the RTC clock. In order to do so, a
 * current time and date are needed. This is provided throught the
 * parameters of the funtion
 * Inputs:
 * 		RTC_Date* - pointer to current data
 * 		RTC_Time* - pointer to current time
 * Outputs:
 * 		none
 */
void init_rtc(RTC_Date* date,RTC_Time* time){
	//enable clock to PWR
	RCC->APB1ENR |= 0x10000000;
	
	//enable access to backup domain
	*(PWR_CR) |= (1<<DBP);
	
	//select RTC clock source-low speed external
	RCC->BDCR |= (1<<16);
	RCC->BDCR &= ~(1<<16);
	//RCC->BDCR &= ~(0b11<<8);
	RCC->BDCR |= (1<<RCC_RTCCLKSource);
	
	//enable LSE
	RCC->CSR |= LSE_ON;
	//enable RTC clock
	RCC->BDCR |= 1<<RTC_ENABLE;
	RCC->BDCR |= 1;
	
	enable_RTC_init();

	//set for 1Hz internal clock and configure RTC precalar
	RTC->PRER = SYNCHPREDIV & 0x7FFF;
	RTC->PRER |= (ASYNCHPREDIV<<16);
	RTC->CR |= (time->timeMode)<<6;
	RTC->CR |= (1<<FMT);			//AM/PM mode
		
	//set time/date
	setDate(date);
	setTime(time);
	
	disable_RTC_init();
}

/**
 * This function is used to set the data
 * Inputs:
 * 		RTC_Date* - current date
 * Outputs:
 * 		none
 */
void setDate(RTC_Date* date){
	uint32_t tempDate = date->RTC_Date | \
						(date->RTC_Month << 8) | \
						(date->RTC_WeekDay << 13) | \
						(date->RTC_Year << 16);
	//disable_RTC_write_protect();
	RTC->DR = tempDate&0x00FFFF3F;
	//enable_RTC_write_protect();
}

/**
 * This function is used to set the data
 * Inputs:
 * 		RTC_Time* - current time
 * Outputs:
 * 		none
 */
void setTime(RTC_Time* time){
	uint32_t tempTime = time->RTC_Seconds | \
						(time->RTC_Minutes << 8) | \
						(time->RTC_Hours << 16) | \
						(time->timeMode << 22);
	//disable_RTC_write_protect();
	RTC->TR = (tempTime & 0x007F7F7F);
	//enable_RTC_write_protect();
}

/**
 * returns a string representation fo the current time
 * Inputs:
 * 		char* - string to populate with current time(at least 12 characters)
 * Outputs:
 * 		char* - result of converted string
 */
char* time_to_string(char* time){
	if(strlen(time)<12){
		uint8_t one_hour = ((RTC->TR) & 0xF0000)>>16;
		uint8_t ten_hour = ((RTC->TR) & 0x300000)>>20;
		uint8_t ten_min = ((RTC->TR) & 0x7000)>>12;
		uint8_t one_min = ((RTC->TR) & 0xF00)>>8;
		uint8_t ten_sec = ((RTC->TR) & 0x70)>>4;
		uint8_t one_sec = (RTC->TR) & 0xF;
		uint8_t am_pm = ((RTC->TR) & 0x400000)>>22;

		uint8_t hours = (ten_hour<<4) | one_hour;
		uint8_t mins = (ten_min<<4) | one_min;
		uint8_t secs = (ten_sec*10)+one_sec;


		sprintf(time, "%d%d:%d%d:%d%d ",ten_hour,one_hour,ten_min,one_min,ten_sec,one_sec);
		if(am_pm){
			strcat(time,"PM");
		}else{
			strcat(time,"AM");
		}

		uint32_t ignore = (RTC->DR);	//need to read date to keep system synced.
	}
	return time;	
}

/**
 * This function will return a numeric representation of the current hour
 * Inputs:
 * 		none
 * Outputs:
 * 		uint8_t - current hour(24 hour time)
 */
uint8_t get_Hour(){
	uint8_t ten_hour = ((RTC->TR) & 0x300000)>>20;
	uint8_t one_hour = ((RTC->TR) & 0xF0000)>>16;
	uint8_t am_pm = ((RTC->TR) & 0x400000)>>22;
	uint8_t hours = (ten_hour*10)+one_hour;
	if(am_pm==1&&hours!=12){//if PM
		hours+=12;
	}
	uint32_t ignore = (RTC->DR);
	return hours;
}

/**
 * Enters the key to unlock RTC registers
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void disable_RTC_write_protect(){
	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
}

/**
 * Locks RTC registers by writing incorrect key
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void enable_RTC_write_protect(){
	//RTC->ISR &= 1<<INIT;
	RTC->WPR = 0x00;
	RTC->WPR = 0x00;
}

/**
 * This function is used to disable the initialize mode of the RTC registers
 * Once this is called, the clock will begin running.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void disable_RTC_init(){
	//exit init mode
	RTC->ISR &= ~(1<<INIT);
}

/**
 * This function will start the initialization period of the RTC circuit.
 * If this is not called, than the RTC cannot be manipulated.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void enable_RTC_init(){
	//enter init mode
	disable_RTC_write_protect();
	RTC->ISR |= (1<<INIT);
	while((RTC->ISR & 1<<INITF) != 1<<INITF){}
	//enable_RTC_write_protect();
}
