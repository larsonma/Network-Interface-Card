/*
 * gpio.h
 *
 *  Created on: Dec 16, 2016
 *      Author: Mitchell Larson
 */

#ifndef GPIO_H
#define GPIO_H
 
#include <inttypes.h>
#include <stddef.h>

//RCC constants 
#define RCC_AHB1ENR (volatile uint32_t*) 	0x40023830
#define GPIOA_RCCEN_F 0
#define GPIOB_RCCEN_F 1
#define GPIOC_RCCEN_F 2

typedef struct{
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint32_t BSSR;
	uint32_t LCKR;
	uint32_t AFRL;
	uint32_t AFRH;
} GPIOx;

//enumerated types
typedef enum {INPUT, OUTPUT, ALTFUNC, ANALOG} Mode;
typedef enum {PUSH_PULL, OPEN_DRAIN} OutputType;
typedef enum {LOW, MED, FAST, HIGH} Speed;
typedef enum {NONE, PULLUP, PULLDOWN} PullType;

//global functions
extern void enable_clock(char port);
extern void set_pin_mode(char port, uint8_t pin, Mode mode);
extern void set_pin_output_type(char port, uint8_t pin, OutputType outtype);
extern void set_output_speed(char port, uint8_t pin, Speed speed);
extern void set_pin_PUPDR(char port, uint8_t pin, PullType pulltype);
extern void set_alt_func(char port, uint8_t pin, uint8_t altFunc);

#endif /* GPIO_H */
