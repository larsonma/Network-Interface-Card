/*
 * main.c
 *
 *  Created on: Feb 17, 2017
 *      Author: Mitchell Larson
 * This is the main application for a system that monitors pedestrian
 * traffic using an infared tripwire. The system can be set to one of
 * 3 different modes. A scanning mode, an alarm mode, and an admin mode
 */

#include "date.h"
#include "timer.h"
#include "piezo.h"
#include "keypad.h"
#include "lcd.h"
#include "RTC.h"
#include <stdlib.h>
#include <string.h>
#include "ADC.h"
#include <stdbool.h>
#include <math.h>

#define NAME_LENGTH 16
#define USERNAME_LENGTH 5
#define PASSWORD_LENGTH 6
#define TOINT 48

typedef enum {INITIALIZE,SCAN,ALARM,ACCESS} TASKMODE;
typedef enum {INCORRECT, CORRECT} Result;

typedef struct{
	char name[NAME_LENGTH+1];
	char username[USERNAME_LENGTH+1];
	char password[PASSWORD_LENGTH+1];
} ADMIN;

static uint32_t doorCount = 0;
static ADMIN mitchell = {"Mitchell","62653","123ABC"};
static bool blocked = false;
static uint32_t hourCount[24] = {0};
static char currentPassword[PASSWORD_LENGTH+1];
static uint8_t passwordIndex = 0;
static bool alarmed = false;

static void promt_for_time();
static void promt_for_date();
static char* get_date();
static char* get_time();
static RTC_Time* conv_time(char*);
static RTC_Date* conv_date(char*);
static void bootUp();
static Result login();
static TASKMODE getCommand();
static void initClock();
static void print_time();
static Result checkPassword();
static void print_scan_status();

/**
 * The main function for this application runs a state machine, while
 * periodically checking for a user entering the admin password. In
 * order to perform tasks, many of the tasks are accomplished by
 * calling static functions within this file
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
int main(void){
	TASKMODE mode = INITIALIZE;
	Note alarm1 = {C,NATURAL,8,250};
	Note alarm2 = {C,NATURAL,6,250};
	Note note = {C,NATURAL,4,250};
	uint32_t current_count = 0;
	while(1){
		switch(mode){
			case INITIALIZE:
				bootUp();
				while(login() == INCORRECT){
					lcd_reset();
					lcd_print_string("Incorrect.");
					delay_ms(500);
				};
				initClock();
				mode = getCommand();
				ADC_init();
				break;
			case SCAN:
				if((current_count+1) == doorCount){	//check if tripwire has been broken
					current_count++;
					hourCount[get_Hour()] = hourCount[get_Hour()] +1;
					play_note(&note);
				}
				print_time();
				break;
			case ALARM:
				if((current_count+1) == doorCount){
					alarmed = true;
				}

				if(alarmed){
					play_note(&alarm1);
					play_note(&alarm2);
				}

				break;
			case ACCESS:
				if((current_count+1) == doorCount){
					current_count++;
					hourCount[get_Hour()] = hourCount[get_Hour()] +1;
					play_note(&note);
				}
				print_scan_status();
				break;
		}

		//check to see if the user has attempted to enter a password to
		//switch modes of the lcd/disable the alarm. Keypad is buffered
		//so there is no risk of losing data
		Result result = checkPassword();
		if(result==CORRECT){
			mode = getCommand();
			passwordIndex = 0;
			alarmed = false;
		}else if(result==INCORRECT){
			passwordIndex = 0;
			lcd_row1();
			lcd_print_string("      ");
			lcd_row1();
		}
	}

	return 0;
}

/**
 * This function will print the prompt to inform the user to enter the
 * time onto the LCD.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void promt_for_time(){
	lcd_reset();
	lcd_print_string("Enter the time:");
	lcd_row1();
	lcd_print_string("  :  :  ");
	delay_ms(1000);
	lcd_row1();
}

/**
 * This function will print the prompt to inform the user to enter the
 * date onto the LCD.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void promt_for_date(){
	lcd_reset();
	lcd_print_string("Enter the date:");
	lcd_row1();
	lcd_print_string("  /  /  ");
	delay_ms(1000);
	lcd_row1();
}

/**
 * This function will get the time from the keyboard entered from the user
 * Memory is then allocated in order to return the string.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static char* get_time(){
	char time[8];
	uint8_t index=0;
	for(int i=0; i<8;i++){
		if(i==2||i==5){
			lcd_data(':');
		}else{
			time[index] = key_getchar();
			lcd_data(time[index]);
			index++;
		}
	}

	lcd_reset();
	lcd_print_string("0 - AM");
	lcd_row1();
	lcd_print_string("1 - PM");

	uint8_t am_pm = -1;
	while(am_pm != 0 && am_pm != 1){
		am_pm = key_getint();
		time[6] = am_pm;
	}

	char* temp= malloc(strlen(time)+1);
	strcpy(temp,time);
	return temp;
}

/**
 * This function will get the date from the keyboard entered from the user
 * Memory is then allocated in order to return the string.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static char* get_date(){
	char date[7];
	uint8_t index=0;
	for(int i=0; i<8;i++){
		if(i==2||i==5){
			lcd_data('/');
		}else{
			date[index] = key_getchar();
			lcd_data(date[index]);
			index++;
		}
	}
	char* temp= malloc(strlen(date)+1);
	strcpy(temp,date);
	return temp;
}

/**
 * This function is used to work with the get_time() function. The string
 * that is returned from that function is parsed, converted out of string
 * form into BCD form. A RTC_Time structure is created with the parsed
 * data, and returned.
 * Inputs:
 * 		char* time_in - the current time in string form
 * Outputs:
 * 		RTC_Time* - pointer to the structure created to represent time.
 */
static RTC_Time* conv_time(char* time_in){
	uint8_t hours = ((uint8_t)((*time_in)-TOINT)<<4) | (*(++time_in)-TOINT);
	uint8_t mins = ((uint8_t)(*(++time_in)-TOINT)<<4) | (*(++time_in))-TOINT;
	uint8_t sec = ((uint8_t)(*(++time_in)-TOINT)<<4) | (*(++time_in))-TOINT;
	uint8_t am_pm = *(++time_in);
	RTC_Time rtcTime = {hours,mins,sec,am_pm};
	RTC_Time* temp = malloc(sizeof(rtcTime));
	memcpy(temp,&rtcTime,sizeof(rtcTime));
	return temp;
}

/**
 * This function is used to work with the get_date() function. The string
 * that is returned from that function is parsed, converted out of string
 * form into BCD form. A RTC_Date structure is created with the parsed
 * data, and returned.
 * Inputs:
 * 		char* date_in - the current date in string form
 * Outputs:
 * 		RTC_Date* - pointer to the structure created to represent the date.
 */
static RTC_Date* conv_date(char* date_in){
	uint8_t month = (((*date_in)-TOINT)<<4) | (*(++date_in)-TOINT);
	uint8_t day = (*((++date_in)-TOINT)<<4) | (*(++date_in))-TOINT;
	uint8_t year = (*((++date_in)-TOINT)<<4) | (*(++date_in))-TOINT;
	RTC_Date rtcDate = {2,month,day,year};
	RTC_Date* temp = malloc(sizeof(rtcDate));
	memcpy(temp,&rtcDate,sizeof(rtcDate));
	return temp;
}

/**
 * This function initializes the RTC clock by getting the date and time
 * from the user, converting it to it's necessary RTC structures, and
 * calling the init_rtc function.
 * Input:
 * 		none
 * Output:
 * 		none
 */
static void initClock(){
	promt_for_date();
	char* date_in = get_date();
	lcd_reset();
	promt_for_time();
	char* time_in = get_time();
	init_rtc(conv_date(date_in),conv_time(time_in));

	free(date_in);
	free(time_in);
}

/**
 * Boot up the necessary peripherals
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void bootUp(){
	init_piezo();
	key_init();
	lcd_init(C_OFF);
}

/**
 * This function prompts a user to login using their username and
 * password. Users are forced to enter their entire user name and
 * password before being checked in order to eliminate brute force
 * hacking. An enumerated type Result is returned based on the result
 * of the login
 * Inputs:
 * 		none
 * Outputs:
 * 		CORRECT - successful login
 * 		INCORRECT - failed login
 */
static Result login(){
	lcd_reset();
	lcd_print_string("Username:");
	lcd_row1();
	char username_in[USERNAME_LENGTH+1];

	for(int i=0;i<strlen(mitchell.username); i++){
		char keyPressed = key_getchar();
		username_in[i] = keyPressed;
		lcd_data(keyPressed);
	}
	username_in[USERNAME_LENGTH] = '\0';
	if(strcmp(username_in,mitchell.username)) return INCORRECT;

	lcd_reset();
	lcd_print_string("Password:");
	lcd_row1();

	char pass_in[PASSWORD_LENGTH+1];
	for(int i=0;i<strlen(mitchell.password); i++){
		pass_in[i] = key_getchar();
		lcd_data('*');
	}
	pass_in[PASSWORD_LENGTH] = '\0';
	if(strcmp(pass_in,mitchell.password)) return INCORRECT;

	lcd_reset();
	return CORRECT;
}

/**
 * This function prompts a user for which mode to run. This is how
 * control is shifted in the state machine. Only valid results can
 * be chosen
 * Inputs:
 * 		none
 * Outputs:
 * 		TASKMODE - mode to switch system to.
 */
static TASKMODE getCommand(){
	lcd_reset();
	lcd_print_string("What should I do?");
	delay_ms(1000);
	uint8_t choice = 0;
	while(choice==0 || choice>3){
		lcd_reset();
		lcd_print_string("1-Scan.");
		lcd_row1();
		lcd_print_string("2-Alarm.");
		delay_ms(1000);
		lcd_reset();
		lcd_print_string("3-Access.");
		delay_ms(1000);
		choice = key_getkey_noblock();
	}
	return choice;
}

/**
 * This function will print the current time to the LCD, but only
 * if there has been a change in time. Otherwise nothing is printed
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void print_time(){
	char time_string[12];
	char previous[12];

	lcd_row0();
	strcpy(time_string,time_to_string(time_string));
	if(strcmp(previous,time_string)){
		lcd_print_string(time_to_string(time_string));
		strcpy(previous,time_string);
	}
}

/**
 * This function will print the customer count if it has changed since
 * the last time this function has been called. The function will also
 * print the busiest hour for the business. Again, this will only be
 * printed if there has been a change.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
static void print_scan_status(){
	static uint32_t prevCount = -1;
	static uint32_t prevHr = -1;
	uint32_t customerCount = ceil(doorCount/2);
	uint32_t busiestHr=0;
	for(int i=0;i<24;i++){
		if(hourCount[i]>hourCount[busiestHr]){
			busiestHr = i;
		}
	}

	if(prevCount!=customerCount){
		prevCount = customerCount;
		lcd_row0();
		lcd_print_string("Tot Cust: ");
		char cnt[20];
		itoa(customerCount,cnt,10);
		lcd_print_string(cnt);
	}

	if(prevHr!=busiestHr){

		prevHr = busiestHr;
		lcd_row1();
		lcd_print_string("Busiest Hr: ");
		char time[20];
		if(busiestHr==12){
			itoa(busiestHr,time,10);
		}else if(busiestHr==0){
			itoa(busiestHr+12,time,10);
		}else if(busiestHr>12){
			itoa(busiestHr-12,time,10);
		}else if(busiestHr<12 ){
			itoa(busiestHr,time,10);
		}

		lcd_print_string(time);
		if(busiestHr < 12){
			lcd_print_string("AM");
		}else{
			lcd_print_string("PM");
		}
	}
}

/**
 * This function will check the if a password is correct. This function
 * will not block so that it can be called in a continous loop. Since
 * the keypad is buffered, only periodically checking the entered
 * password will be okay.
 * Inputs:
 * 		none
 * Outputs:
 * 		Result - CORRECT, INCORRECT
 * 		-1 - password entry not complete
 */
static Result checkPassword(){
	char keyPressed = key_getchar_noblock();
	if(keyPressed!=0){
		currentPassword[passwordIndex] = keyPressed;
		lcd_set_position(1,passwordIndex);
		lcd_data('*');
		passwordIndex++;
		if(passwordIndex==PASSWORD_LENGTH){
			currentPassword[passwordIndex] = '\0';
			if(strcmp(currentPassword,mitchell.password)==0){
				return CORRECT;
			}else{
				return INCORRECT;
			}
		}
	}
	return -1;
}

void ADC_IRQHandler(void){
	//clear interupt flag
	*(ADC_SR) &= ~(1<<1);
	
	//load data from data register
	uint16_t data = *(ADC_DR) & 0xFFFF;
	float mV = ((3.3*data)/4095)*1000;
	if(mV<250){
		if(!blocked){
			blocked = true;
			doorCount++;
		}
	}else{
		if(blocked){
			blocked = false;
		}
	}
}

