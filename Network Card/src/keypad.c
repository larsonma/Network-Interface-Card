/*
 * keypad.c
 *
 *  Created on: Jan. 6 2017
 *      Author: Mitchell Larson
 *
 * This file implements an API for using the keypad. Several functions are implemented
 * to make use easy.
 */

#include "keypad.h"

const char keys[] = "123A456B789C*0#D";
const int integers[] = {1,2,3,10,4,5,6,11,7,8,9,12,14,0,15,13};

static void setRows_readCol();
static void setCol_readRows();
static uint8_t getRow(uint8_t rows);
static uint8_t getCol(uint8_t cols);
static void disable_keys_interrupt();
static void enable_keys_interrupt();

static volatile GPIOx *GPIOC = (GPIOx *) 0x40020800;
static volatile RingBuffer* rowBuffer;
static volatile RingBuffer* colBuffer;

static uint8_t col_code;
static uint8_t row_code;

/*
 * This function initializes the keyboard by enabling the clock the keyboard
 * resides on and setting all pins associated with the keyboard to pull up.
 * This will cause pins to default to logic high 1 when reading pins. The
 * pins are then set to 0 in case they were previously changed prior to use
 * by the keyboard
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void key_init(){
	//setup recieveBuffer
	static volatile RingBuffer temp1 = {0,0};
	static volatile RingBuffer temp2 = {0,0};
	rowBuffer = &temp1;
	colBuffer = &temp2;

	//enable clock
	enable_clock('C');
	
	//set pins to pullup
	for(int i=0;i<=7;i++){
		set_pin_PUPDR('C',i,PULLUP);
	}
	
	//enable clock for SysCFG
	*(RCC_APB2ENR) |= 1<<14;
	
	//connect PC0-3 to EXTI0-3, PC4-7 to EXTI4-7
	*(SYSCFG_EXTICR1) = (uint16_t)(0x2222);
	
	//unmask EXTI0-7 in EXTI
	*(EXTI_IMR) |= 0x0F;
	
	//set falling edge
	*(EXTI_FTSR) |= 0x0F;
	
	//enable Interrupt in NVIC
	//IRQ0-3 is 6th-9th bit in first ISR Reg.
	*(NVIC_ISER0) |= 0b1111<<6;
	
	//write 0's to key pins(4-7)
	GPIOC->ODR &= ~(0xFF);
	
	setRows_readCol();
}

/*
 * This function will retrieve the key being pressed when the function is called.
 * This function will not block if no key is pressed, returning a 0 if no key is
 * pressed.
 * Inputs:
 * 		none
 * Outputs:
 * 		number 0-16 corresponding to key pressed. starts from top left and runs left to
 * 			right, top to bottom.
 */
uint8_t key_getkey_noblock(){
	if(hasElement(rowBuffer) && hasElement(colBuffer)){
		uint8_t row = getRow(get(rowBuffer));						//get the int representation of the row
		uint8_t col = getCol(get(colBuffer));						//get the int representation of the col
		return ((row-1)*4)+col;	
	}else{
		return 0;
	}
}

/*
 * This function returns a number corresponding to the key that was pressed on the
 * keypad. If no key is pressed, the function will block until a keypress is detected.
 * Then, the function will block until the key is released.
 * Inputs:
 * 		none
 * Outputs:
 * 		number 1-16 corresponding to key pressed. See previous funtion for description.
 */
uint8_t key_getkey(){	
	uint8_t row = getRow(get(rowBuffer));						//get the int representation of the row
	uint8_t col = getCol(get(colBuffer));						//get the int representation of the col
	
	return ((row-1)*4)+col;							//return the number associated with key(1-16)	
}

/*
 * This function returns the ascii character corresponding to the key pressed. This function will
 * block until a key is pressed and released. The ascii character is determined by indexing the
 * character array, keyPressed.
 * Inputs:
 * 		none
 * Outputs:
 * 		ascii character corresponding to key pressed.
 */
char key_getchar(){
	uint8_t keyPressed = key_getkey();
	return keys[keyPressed-1];
}

/*
 * This function returns the ascii character corresponding to the key pressed. This function wont
 * block until a key is pressed and released. The ascii character is determined by indexing the
 * character array, keyPressed. If no key is pressed, 0 is returned
 * Inputs:
 * 		none
 * Outputs:
 * 		ascii character corresponding to key pressed.
 */
char key_getchar_noblock(){
	uint8_t keyPressed = key_getkey_noblock();
	if(keyPressed!=0){
		return keys[keyPressed-1];
	}
	return 0;
}

/*
 * This function returns the interger number associated with the key. This is different from
 * the regular getKey as pressing 9 will return the value 9 in this function. This function
 * will block until a key is pressed.
 * Inputs:
 * 		none
 * Outputs:
 * 		integer corresponding to the key pressed.
 */
uint8_t key_getint(){
	uint8_t keyPressed = key_getkey();
	return integers[keyPressed-1];
}


static void setRows_readCol(){
	//set the rows to output mode
	for(int i=4;i<=7;i++){
		set_pin_mode('C',i,OUTPUT);
	}
	
	//set columns to input
	for(int i=0;i<=3;i++){
		set_pin_mode('C',i,INPUT);
	}
}

static void setCol_readRows(){
	//set the rows to input mode
	for(int i=4;i<=7;i++){
		set_pin_mode('C',i,INPUT);
	}
	
	//set columns to output
	for(int i=0;i<=3;i++){
		set_pin_mode('C',i,OUTPUT);
	}
}

static void disable_keys_interrupt(){
	//*(NVIC_ICER0) |= 0b1111<<6;
	*(EXTI_IMR) &= ~(0x0F);
}

static void enable_keys_interrupt(){
	//*(NVIC_ISER0) |= 0b1111<<6;
	*(EXTI_IMR) |= 0x0F;
}

static uint8_t getRow(uint8_t rows){
	uint8_t row = -1;
	
	//return row number corresponding to the row encoding
	switch(rows){
		case 0b1111:
			row=0;
			break;
		case 0b1110:
			row=1;
			break;
		case 0b1101:
			row=2;
			break;
		case 0b1011:
			row=3;
			break;
		case 0b0111:
			row=4;
			break;
		default:
			row=0;
			break;			
	}
	
	return row;
}

static uint8_t getCol(uint8_t cols){
	int col = -1;
	
	//return column corresponding to column encoding
	switch(cols){
		case 0b1111:
			col=0;
			break;
		case 0b1110:
			col=1;
			break;
		case 0b1101:
			col=2;
			break;
		case 0b1011:
			col=3;
			break;
		case 0b0111:
			col=4;
			break;
		default:
			col=0;
			break;			
	}
	
	return col;
}

//ISR functions to handle a key being pressed

void EXTI0_IRQHandler(void){
	//clear interrupt
	*(EXTI_PR) |= 0b1;
	if(hasSpace(colBuffer) && hasSpace(rowBuffer)){
		disable_keys_interrupt();
	
		//save column encoding
		for(int i=0;i<=3;i++){
			set_pin_mode('C',i,INPUT);
		}
		
		put(colBuffer,GPIOC->IDR & 0x0F);
		
		//read rows, set columns
		setCol_readRows();
		
		put(rowBuffer,(GPIOC->IDR & 0xF0)>>4);
		
		//set rows to output, read columns
		setRows_readCol();

		enable_keys_interrupt();
	}	
}

void EXTI1_IRQHandler(void){
	//clear interrupt
	*(EXTI_PR) |= 0b1<<1;
	if(hasSpace(colBuffer) && hasSpace(rowBuffer)){
		disable_keys_interrupt();
	
		//save column encoding
		for(int i=0;i<=3;i++){
			set_pin_mode('C',i,INPUT);
		}
		
		put(colBuffer,GPIOC->IDR & 0x0F);
		
		//read rows, set columns
		setCol_readRows();
		
		put(rowBuffer,(GPIOC->IDR & 0xF0)>>4);
		
		//set rows to output, read columns
		setRows_readCol();

		enable_keys_interrupt();
	}	
}

void EXTI2_IRQHandler(void){
	//clear interrupt
	*(EXTI_PR) |= 0b1<<2;
	if(hasSpace(colBuffer) && hasSpace(rowBuffer)){
		disable_keys_interrupt();
	
		//save column encoding
		for(int i=0;i<=3;i++){
			set_pin_mode('C',i,INPUT);
		}
		
		put(colBuffer,GPIOC->IDR & 0x0F);
		
		//read rows, set columns
		setCol_readRows();
		
		put(rowBuffer,(GPIOC->IDR & 0xF0)>>4);
		
		//set rows to output, read columns
		setRows_readCol();

		enable_keys_interrupt();
	}	
}

void EXTI3_IRQHandler(void){
	//clear interrupt
	*(EXTI_PR) |= 0b1<<3;
	if(hasSpace(colBuffer) && hasSpace(rowBuffer)){
		disable_keys_interrupt();
	
		//save column encoding
		for(int i=0;i<=3;i++){
			set_pin_mode('C',i,INPUT);
		}
		
		put(colBuffer,GPIOC->IDR & 0x0F);
		
		//read rows, set columns
		setCol_readRows();
		
		put(rowBuffer,(GPIOC->IDR & 0xF0)>>4);
		
		//set rows to output, read columns
		setRows_readCol();

		enable_keys_interrupt();
	}	
}







