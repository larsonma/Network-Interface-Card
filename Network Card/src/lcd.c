/*
 * lcd.c
 *
 *  Created on: Dec 16, 2016
 *      Author: larsonma
 *
 * This file implements an API for the LCD to allow for easy use of the LCD.
 */

#include "lcd.h"

void static set_upper_nibble(uint8_t command);
void static set_lower_nibble(uint8_t command);
void static latch();
void static lcd_execute(uint8_t command);
void static poll_busy();



/*
 * Initializes the LCD to 4 line mode so that its following
 * methods can be run to create ease of use of interfacing with
 * outside viewers
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void lcd_init(Cursor_Mode mode){
	
	//wait 40ms for the LCD display to power on
	delay_ms(40);
	
	//enable gpio b and gpio c ports
	enable_clock('B');
	enable_clock('C');
	
	//set port B pins 0-2 to output mode
	for(int i=0;i<=2;i++){
		set_pin_mode('B',i,OUTPUT);
	}
	
	//set port C pins 8-11 to output mode
	for(int i =8;i<=11;i++){
		set_pin_mode('C',i,OUTPUT);
	}
	
	lcd_cmd(0x30);	//set to 8 pin mode by sending command 0x30
	lcd_cmd(0x28);	//set to 4 pin mode by sending command 0x28
	lcd_clear();	//clear by sending command 0x01
	lcd_home();		//move home by sending command 0x02
	lcd_cmd(0x06);	//set entry mode, move right, no shift, command 0x06
	if(mode==C_ON){
		lcd_cmd(0x0D);	//display on, no cursor, blinking, 0x0D
	}else{
		lcd_cmd(0x0C);
	}
}

/*
 * This function allows a user to clear the lcd by sending the clear command to
 * the LCD controller
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void lcd_clear(){
	lcd_cmd(0x01);
}

/*This function allows a user to return the cursor on the LCD to the home position(row 0, col 0)
 *by sending the home command to the LCD controller.
 *Inputs:
 *		none
 *Outputs:
 *		none
 */
void lcd_home(){
	lcd_cmd(0x02);
}

/**
 * This function allows a user to clear the LCD, while also returning to the 0 row and 0 col
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void lcd_reset(){
	lcd_clear();
	lcd_home();
}

/**
 * This function moves the cursor to line 0 col 0
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void lcd_row0(){
	lcd_set_position(0,0);
}

/**
 * This function moves the cursor to line 1 col 0
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void lcd_row1(){
	lcd_set_position(1,0);
}

/*
 * This function allows a user to set the cursor to a specified row and column on the LCD. Both
 * the row and the column are 0 based. Protection from invalid row and column implemented.
 * Inputs:
 * 		row - row to set cursor to(0 based)
 * 		col - column to set cursor to(0 based)
 * Outputs:
 * 		none
 */
void lcd_set_position(uint8_t row,uint8_t col){
	if(row >= 0 && col >= 0){
		lcd_home();			//send the LCD home

		//determine amount of times for lcd to shift by multiplying by the
		//amount of characters per line(40) and adding the columns to the result
		uint32_t shftAmt = (row*40) + col;

		//shift the cursor shftAmt times using the lcd command for shifting
		for(int i=0;i<shftAmt;i++){
			lcd_cmd(0b10100);
		}
	}
}

/*
 * This function accepts a pointer to a String(character array) and prints the String to
 * the lcd. Once the String is printed, the length of the string is returned.
 * Inputs:
 * 		*pointer - pointer to the character array to be printed
 * Outputs:
 * 		length of string/character array.
 */
int lcd_print_string(const char *pointer){
	int length = 0;
	for(;*pointer!='\0';pointer++){
		lcd_data(*pointer);
		length++;
		//delay_us(100);
	}
	return length;
}

/*
 * This function accepts an integer and prints the integer to the screen in BCD form.
 * Inputs:
 * 		int num - integer to print to lcd
 * Outputs:
 * 		number of digits in number printed to lcd.
 */
int lcd_print_num(int num){
	if(num<0){								//check if number is less than 0. If so, print '-'
		lcd_data('-');						// and multiply number by -1.
		num *= -1;
	}else if(num==0){						//if number is 0, print 0
		lcd_data(0+48);
		return 1;
	}
	
	char asciiNum[MAX_INT+1];				//create array to store BCD representation
	int count=0;							//initialize count to 0
	while(num!=0){							//if number doesn't equal 0, execute algorithm
		int result = num%10;
		num = (num-result)/10;
		asciiNum[count] = (result+48);		//convert to ascii to print number
		count++;
	}
		
	int length=0;
	for(--count;count>=0;count--){			//pop ascii characters off array
		lcd_data(asciiNum[count]);			//print ascii representation of number
		length++;
	}
	
	return length;
}

/*
 * This function allows a user to execute a command for the lcd to execute. It is the
 * responsibility of the user to verify that the command is valid.
 * Inputs:
 * 		uint8_t command - command to send to lcd controller
 * Outputs:
 * 		none
 */
void lcd_cmd(uint8_t command){
	//make sure rw and rs are low
	GPIOB->ODR &= ~(0b11);	//clear the rw and rs bits
	
	//send the command
	lcd_execute(command);
}

/*
 * This function allows a user to print raw data to the lcd. It is the
 * responsibility of the user to verify that the data is valid.
 * Inputs:
 * 		uint8_t data - data to send to lcd controller
 * Outputs:
 * 		none
 */
void lcd_data(uint8_t data){
	//make sure rw is low and rs is high
	GPIOB->ODR &= ~(0b11);			//clear the rw and rs bits
	GPIOB->ODR |= (1<<LCD_RS_F);	//set rs high
	
	//send data
	lcd_execute(data);
}

void static lcd_execute(uint8_t command){
	//ensure data pins are set to output mode
	for(int i =8;i<=11;i++){
		set_pin_mode('C',i,OUTPUT);
	}
	
	set_upper_nibble(command);
	latch();
	set_lower_nibble(command);
	latch();
	poll_busy();
}

static void set_upper_nibble(uint8_t command){
	GPIOC->ODR &= ~(0b1111 << LCD_DATA_OFFSET);	//clear the PortC 8-11 bits
	
	//logical shift the most significant nibble right by 4 to elimintate the 
	//least significant nibble. Then shift it into the data transfer bits.
	GPIOC->ODR |= ((command >> 4) << LCD_DATA_OFFSET);
}

static void set_lower_nibble(uint8_t command){
	GPIOC->ODR &= ~(0b1111 << LCD_DATA_OFFSET);	//clear the PortC 8-11 bits
	command &= ~(0b1111<<4);						//clear the upper nibble
	GPIOC->ODR |= (command << LCD_DATA_OFFSET); 	//shift lower nibble to data transfer bits.
}

static void latch(){
	GPIOB->ODR |= (1<<LCD_E_F);	//bring E high(pin 2)
	delay_us(1);					//delay 1 microsecond to latch
	GPIOB->ODR &= ~(1<<LCD_E_F);	//bring E low
	delay_us(1);					//let E settle
}

static void poll_busy(){
	//set RS low and R/W high
	GPIOB->ODR &= ~(0b11);
	GPIOB->ODR |= (1<<LCD_RW_F);
	
	//set pin 11 on port C to input mode
	set_pin_mode('C',11,INPUT);
	
	//delay 80 us
	delay_us(85);
	
	//loop until busy flag is 0
	uint8_t bf = 1;
	while(bf!=0){
		GPIOB->ODR |= (1<<LCD_E_F);	//bring E high(pin 2)
		delay_us(1);					//delay 1 microsecond to latch
		if((GPIOC->IDR &= (1<<11))!=(1<<11)){//check bf
			bf = 0;
		}
		GPIOB->ODR &= ~(1<<LCD_E_F);	//bring E low
		delay_us(1);					//let E settle
		latch();		//latch
	}
}


