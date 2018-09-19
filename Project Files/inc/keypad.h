/*
 * keypad.h
 *
 *  Created on: Jan. 6 2017
 *      Author: Mitchell Larson
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <inttypes.h>
#include "gpio.h"
#include "ringbuffer.h"

#define RCC_APB2ENR (volatile uint32_t*) 0x40023844
#define SYSCFG_EXTICR1 (volatile uint32_t*) 0x40013808
#define SYSCFG_EXTICR2 (volatile uint32_t*) 0x4001380C
#define SYSCFG_EXTICR3 (volatile uint32_t*) 0x40013810

#define EXTI_FTSR (volatile uint32_t*) 0x40013C0C
#define NVIC_ISER0 (volatile uint32_t*) 0xE000E100
#define NVIC_ICER0 (volatile uint32_t*) 0xE000E180
#define EXTI_PR (volatile uint32_t*) 0x40013C14
#define EXTI_IMR (volatile uint32_t*) 0x40013C00

//global functions
extern void key_init();
extern uint8_t key_getkey_noblock();
extern uint8_t key_getkey();
extern char key_getchar();
extern uint8_t key_getint();
extern char key_getchar_noblock();

#endif /* KEYPAD_H */
