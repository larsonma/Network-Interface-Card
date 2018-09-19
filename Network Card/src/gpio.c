/*
 * gpio.c
 *
 *  Created on: Dec 16, 2016
 *      Author: larsonma
 *  This is an API designed to make GPIO initialization and modification easy to
 *  use, while minimizing risk of mistakes. Currently works for ports A,B, and C
 */
 
#include "gpio.h"

static volatile GPIOx *GPIOA = (GPIOx *) 0x40020000;
static volatile GPIOx *GPIOB = (GPIOx *) 0x40020400;
static volatile GPIOx *GPIOC = (GPIOx *) 0x40020800;

static void set_low_reg_alt_func(char port, uint8_t pin, uint8_t altFunc);
static void set_high_reg_alt_func(char port, uint8_t pin,uint8_t altFunc);
static volatile GPIOx* assignPort(char port);

/*
 * The function allows the clock for a specific GPIO port to be initialized. The port to
 * be specified is not case sensitive.
 * Inputs:
 * 		char port - port to enable clock for
 * Outputs:
 * 		none
 */
void enable_clock(char port){
	switch(port){
		case 'A':
		case 'a':
			*(RCC_AHB1ENR) |= (1<<GPIOA_RCCEN_F);
			break;
		case 'B':
		case 'b':
			*(RCC_AHB1ENR) |= (1<<GPIOB_RCCEN_F);
			break;
		case 'C':
		case 'c':
			*(RCC_AHB1ENR) |= (1<<GPIOC_RCCEN_F);
			break;			
	}
}

/*
 * This function allows a user to set any pin on a port to one of the 4 modes. The modes
 * are specified by enumerated types INPUT, OUTPUT, ALTFUNC, and ANALOG. Protection against
 * setting invalid pins is implemented.
 * Inputs:
 * 		char port - port to modify pins on
 * 		uint8_t pin - pin to be modified
 * 		Mode mode - mode to set pin to.
 * Outputs:
 * 		none
 */
void set_pin_mode(char port, uint8_t pin, Mode mode){
	if(pin >=0 && pin <= 15){
		volatile GPIOx* portX = assignPort(port);
		
		//check to see if port is null
		if(!portX) return;
		
		//clear pins associated with pin mode
		portX->MODER &= ~(0b11<<(pin*2));
		
		//set pin according to MODE specified
		switch(mode){
			case INPUT:
				portX->MODER |= (0b00<<(pin*2));
				break;
			case OUTPUT:
				portX->MODER |= (0b01<<(pin*2));
				break;
			case ALTFUNC:
				portX->MODER |= (0b10<<(pin*2));
				break;
			case ANALOG:
				portX->MODER |= (0b11<<(pin*2));
				break;
			default:
				return;
		}
	}
}

/*
 * This function allows a user to set any pin on a port to one of the 2 output types. The types
 * are specified by enumerated types PULLUP and OPEN_DRAIN. Protection against
 * setting invalid pins is implemented.
 * Inputs:
 * 		char port - port to modify pins on
 * 		uint8_t pin - pin to be modified
 * 		OutputType outtype - type to set pin to.
 * Outputs:
 * 		none
 */
void set_pin_output_type(char port, uint8_t pin, OutputType outtype){
	if(pin >= 0 && pin <= 15){
		volatile GPIOx* portX = assignPort(port);
		
		//check to see if port is null
		if(!portX) return;

		//clear pin specified
		portX->OTYPER &= ~(1<<pin);

		//switch output type of pin to type specified.
		switch(outtype){
			case PUSH_PULL:
				portX->OTYPER |= (0<<pin);
				break;
			case OPEN_DRAIN:
				portX->OTYPER |= (1<<pin);
				break;
			default:
				return;
		}
	}		
}

/*
 * This function allows a user to set any pin on a port to one of the 4 speeds. The speeds
 * are specified by enumerated types LOW, MED, FAST, and HIGH. Protection against
 * setting invalid pins is implemented.
 * Inputs:
 * 		char port - port to modify pins on
 * 		uint8_t pin - pin to be modified
 * 		Speed speed - speed to set pin to.
 * Outputs:
 * 		none
 */
void set_output_speed(char port, uint8_t pin, Speed speed){
	if(pin >= 0 && pin <= 15){
		volatile GPIOx* portX = assignPort(port);
		
		//check to see if port is null
		if(!portX) return;

		//clear speed set at specified pin
		portX->OSPEEDR &= ~(0b11<<(pin*2));

		//set pin to specified speed
		switch(speed){
			case LOW:
				portX->OSPEEDR |= (0b00<<(pin*2));
				break;
			case MED:
				portX->OSPEEDR |= (0b01<<(pin*2));
				break;
			case FAST:
				portX->OSPEEDR |= (0b10<<(pin*2));
				break;
			case HIGH:
				portX->OSPEEDR |= (0b11<<(pin*2));
				break;
			default:
				return;
		}
	}		
}

/*
 * This function allows a user to set any pin on a port to one of the 3 pull types. The types
 * are specified by enumerated types NONE, PULLUP, and PULLDOWN. Protection against
 * setting invalid pins is implemented.
 * Inputs:
 * 		char port - port to modify pins on
 * 		uint8_t pin - pin to be modified
 * 		PullType pulltype - pulltype to set pin to.
 * Outputs:
 * 		none
 */
void set_pin_PUPDR(char port, uint8_t pin, PullType pulltype){
	if(pin >= 0 && pin <= 15){
		volatile GPIOx* portX = assignPort(port);
		
		//check to see if portX is null
		if(!portX) return;

		//clear pull type set at specified pin
		portX->PUPDR &= ~(0b11<<(pin*2));

		//set pin the the specified pull type
		switch(pulltype){
			case NONE:
				portX->PUPDR |= (0b00<<(pin*2));
				break;
			case PULLUP:
				portX->PUPDR |= (0b01<<(pin*2));
				break;
			case PULLDOWN:
				portX->PUPDR |= (0b10<<(pin*2));
				break;
			default:
				return;
		}
	}		
}

/*
 * This function allows a user to set any pin on a port to one of the 15 alternate modes. The modes
 * are specified by their respective number specified in the programmers manual. Protection against
 * setting invalid pins is implemented.
 * Inputs:
 * 		char port - port to modify pins on
 * 		uint8_t pin - pin to be modified
 * 		uint8_t altFunc - alternate function to set pin to.
 * Outputs:
 * 		none
 */
void set_alt_func(char port, uint8_t pin, uint8_t altFunc){
	if(pin >= 0 && pin <=7){
		set_low_reg_alt_func(port,pin,altFunc);		//if pin corresponds to pin 0-7, use the alternate function low register
	}else if(pin >=8 && pin <= 15){
		set_high_reg_alt_func(port,pin,altFunc);	//if pin corresponds to pin 8-15, use the alternate function high register
	}else{
		return;
	}
}

static void set_low_reg_alt_func(char port, uint8_t pin, uint8_t altFunc){
	volatile GPIOx* portX = assignPort(port);
		
	//check to see if port is null
	if(!portX) return;
			
	portX->AFRL &= ~(0b1111<<(pin*4));

	switch(altFunc){
		case 0:
			portX->AFRL |= (0b0000<<(pin*4));
			break;
		case 1:
			portX->AFRL |= (0b0001<<(pin*4));
			break;
		case 2:
			portX->AFRL |= (0b0010<<(pin*4));
			break;
		case 3:
			portX->AFRL |= (0b0011<<(pin*4));
			break;
		case 4:
			portX->AFRL |= (0b0100<<(pin*4));
			break;
		case 5:
			portX->AFRL |= (0b0101<<(pin*4));
			break;
		case 6:
			portX->AFRL |= (0b0110<<(pin*4));
			break;
		case 7:
			portX->AFRL |= (0b0111<<(pin*4));
			break;
		case 8:
			portX->AFRL |= (0b1000<<(pin*4));
			break;
		case 9:
			portX->AFRL |= (0b1001<<(pin*4));
			break;
		case 10:
			portX->AFRL |= (0b1010<<(pin*4));
			break;
		case 11:
			portX->AFRL |= (0b1011<<(pin*4));
			break;
		case 12:
			portX->AFRL |= (0b1100<<(pin*4));
			break;
		case 13:
			portX->AFRL |= (0b1101<<(pin*4));
			break;
		case 14:
			portX->AFRL |= (0b1110<<(pin*4));
			break;
		case 15:
			portX->AFRL |= (0b1111<<(pin*4));
			break;
		default:
			return;
	}
}

static void set_high_reg_alt_func(char port, uint8_t pin,uint8_t altFunc){
	pin -= 8;
	
	volatile GPIOx* portX = assignPort(port);
		
	//check to see if port is null
	if(!portX) return;
				
	portX->AFRH &= ~(0b1111<<(pin*4));

	switch(altFunc){
		case 0:
			portX->AFRH |= (0b0000<<(pin*4));
			break;
		case 1:
			portX->AFRH |= (0b0001<<(pin*4));
			break;
		case 2:
			portX->AFRH |= (0b0010<<(pin*4));
			break;
		case 3:
			portX->AFRH |= (0b0011<<(pin*4));
			break;
		case 4:
			portX->AFRH |= (0b0100<<(pin*4));
			break;
		case 5:
			portX->AFRH |= (0b0101<<(pin*4));
			break;
		case 6:
			portX->AFRH |= (0b0110<<(pin*4));
			break;
		case 7:
			portX->AFRH |= (0b0111<<(pin*4));
			break;
		case 8:
			portX->AFRH |= (0b1000<<(pin*4));
			break;
		case 9:
			portX->AFRH |= (0b1001<<(pin*4));
			break;
		case 10:
			portX->AFRH |= (0b1010<<(pin*4));
			break;
		case 11:
			portX->AFRH |= (0b1011<<(pin*4));
			break;
		case 12:
			portX->AFRH |= (0b1100<<(pin*4));
			break;
		case 13:
			portX->AFRH |= (0b1101<<(pin*4));
			break;
		case 14:
			portX->AFRH |= (0b1110<<(pin*4));
			break;
		case 15:
			portX->AFRH |= (0b1111<<(pin*4));
			break;
		default:
			return;
	}
}

static volatile GPIOx* assignPort(char port){
	switch(port){
		case 'A':
		case 'a':
			return GPIOA;
		case 'B':
		case 'b':
			return GPIOB;
		case 'C':
		case 'c':
			return GPIOC;
		default:
			return NULL;		//error
	}
}



