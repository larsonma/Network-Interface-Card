/*
 * ADC.c
 *
 *  Created on: Jan 11, 2017
 *      Author: Mitchell Larson
 */

#include "ADC.h"

static void init_clock();

/**
 * This function initializes the ADC by enable the clock for the ADC and
 * GPIO port A. Pin 6 for port A is then set to analog mode, the channel
 * for the converter is selected, and the converter is turned on.
 * The converter is set up to analyze the temperature sensor.
 * The ADC is also set up to be driven by interrupt
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void ADC_init(){
	//enable clock for ADC1
	*(APB2ENR) |= 1<<8;
	
	//enable clock for GPIOA
	enable_clock('A');
	
	//enable analog mode for GPIOA, pin6
	set_pin_mode('A', 5, ANALOG);
	
	//SELECT CHANNEL
	*(ADC_SQR3) = 5;
	
	//enable EOC interrupt
	*(ADC_CR1) |= 1<<5;
	
	//enable interupt on NVIC(position 18)
	*(NVIC_ISER0) |= 1<<18;
	
	//init clock
	init_clock();
	
	//turn on ADC1
	*(ADC_CR2) |= 1;
}

static void init_clock(){
	//enable clock for TIM2
	*(APB1ENR) |= 1;
	
	//set clock prescalar
	*(TIM2_PSC) = 16000;		//1000 counts = 1 second
	
	//reload set to 100 ms
	*(TIM2_ARR) = (100-1);
	
	//compare to 100 ms
	*(TIM2_CCR2) = (100-1);
	
	//select output mode (0b001)
	*(TIM2_CCMR1) &= ~(0b111<<12);
	*(TIM2_CCMR1) |= 0b001<<12;
	
	//make OC2 signal an output on corresponding pin5
	*(TIM2_CCER) |= 1<<4;
	
	//enable interrupt
	*(TIM2_DIER) |= 1<<2;
	
	//enable in NVIC
	*(NVIC_ISER0) |= 1<<28;
	
	//enable the counter by setting CEN bit in CR1
	*(TIM2_CR1) |= 1;
}

/**
 * This function will start a conversion on the ADC when called and then
 * return the data in the data register. The upper half word is cleared
 * in the DR to ensure no garbage data is returned.
 * Inputs:
 * 		none
 * Outputs:
 * 		data in the DR register
 */
uint32_t take_sample(){
	//start conversion by setting SWSTART bit in ADC_CR2
	*(ADC_CR2) |= (1<<30);
	
	//wait for EOC bit to be set
	while((*(ADC_SR) & (1<<1))==0){}
	
	//return data in ADC_DR
	return (*(ADC_DR) & 0xFFFF);
}

/**
 * This function will return a temperature representation of the data in
 * the data register in celcius using previously defined functions
 * Inputs:
 * 		none
 * Outputs:
 * 		temperature in celcius
 */
float get_tempC(){
	return 25+((get_mili_volts()-750)/10);
}

/**
 * This function will return a temperature representation of the data in
 * the data register in ferenheit using previously defined functions
 * Inputs:
 * 		none
 * Outputs:
 * 		temperature in Ferenheit
 */
float get_tempF(){
	return (get_tempC()*1.8)+32;
}

/**
 * This function will return the milivolts of the incoming signal from
 * the temperature sensor.
 * Inputs:
 * 		none
 * Outputs:
 * 		milivolts
 */
float get_mili_volts(){
	return (((take_sample()*3.3)/4095)*1000);
}

void TIM2_IRQHandler(void){
	//clear flag
	*(TIM2_SR) &= ~(1<<2);
	
	//start conversion
	*(ADC_CR2) |= 1<<30;
}
