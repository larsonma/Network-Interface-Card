/*
 * ADC.h
 *
 *  Created on: Jan 11, 2017
 *      Author: Mitchell Larson
 */

#ifndef ADC_H
#define ADC_H

//ADC constants
#define ADC_BASE 	(volatile uint32_t*)	0x40012000
#define ADC_SR 	 	(volatile uint32_t*)	0x40012000
#define ADC_CR1	 	(volatile uint32_t*)	0x40012004
#define ADC_CR2  	(volatile uint32_t*)	0x40012008
#define ADC_SMPR1	(volatile uint32_t*)	0x4001200C
#define ADC_SMPR2	(volatile uint32_t*)	0x40012010
#define ADC_HTR		(volatile uint32_t*)	0x40012024
#define ADC_LTR		(volatile uint32_t*)	0x40012028
#define ADC_SQR1	(volatile uint32_t*)	0x4001202C
#define ADC_SQR2	(volatile uint32_t*)	0x40012030
#define ADC_SQR3	(volatile uint32_t*)	0x40012034
#define ADC_DR		(volatile uint32_t*)	0x4001204C

//RCC constants
#define RCC_BASE	(volatile uint32_t*)	0x40023800
#define APB2ENR		(volatile uint32_t*)	0x40023844
#define APB1ENR		(volatile uint32_t*)	0x40023840

//NVIC constants
#define NVIC_ISER0 (volatile uint32_t*)		0xE000E100

//TIM2 constants
#define TIM2_PSC 	(volatile uint32_t*)	0x40000028
#define TIM2_ARR	(volatile uint32_t*)	0x4000002C
#define TIM2_CCR1	(volatile uint32_t*)	0x40000034
#define TIM2_CCR2	(volatile uint32_t*)	0x40000038
#define TIM2_CCMR1	(volatile uint32_t*)	0x40000018
#define TIM2_CCER	(volatile uint32_t*)	0x40000020
#define TIM2_CR1	(volatile uint32_t*)	0x40000000
#define TIM2_EGR	(volatile uint32_t*)	0x40000014
#define TIM2_DIER	(volatile uint32_t*)	0x4000000C
#define TIM2_SR		(volatile uint32_t*)	0x40000010
#define TIM2_CNT	(volatile uint32_t*)	0x40000024


#include <inttypes.h>
#include "gpio.h"

extern void ADC_init();
extern uint32_t take_sample();
extern float get_tempC();
extern float get_tempF();
extern float get_mili_volts();

#endif /* ADC_H */
