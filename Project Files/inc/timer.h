/*
 * timer.h
 *
 *  Created on: Jan. 5 2017
 *      Author: Mitchell Larson
 */

#ifndef TIMER_H
#define TIMER_H


//SysTic constants
#define STK_CTRL (volatile uint32_t*) 0xE000E010
#define STK_LOAD (volatile uint32_t*) 0xE000E014
#define STK_VAL (volatile uint32_t*) 0xE000E018
#define STK_ENABLE_F 0
#define STK_CLKSOURCE_F 2
#define STK_CNTFLAG_F 16

#include <inttypes.h>

extern void delay_ms(uint32_t t_ms);
extern void delay_us(uint32_t t_us);

#endif /* TIMER_H */