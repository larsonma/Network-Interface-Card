/*
 * lcd.h
 *
 *  Created on: Jan. 5 2017
 *      Author: Mitchell Larson
 */

#ifndef LCD_H
#define LCD_H

//included libraries
#include <inttypes.h>
#include "gpio.h"
#include <stdio.h>
#include "timer.h"

static volatile GPIOx *GPIOB = (GPIOx *) 0x40020400;
static volatile GPIOx *GPIOC = (GPIOx *) 0x40020800;

 
//RCC constants
#define RCC_AHB1ENR (volatile uint32_t*) 0x40023830
#define GPIOB_EN_F 1
 
//GPIO constants
#define GPIOB_BASE (volatile uint32_t*) 0x40020400

//LCD constants
#define LCD_DATA_OFFSET  8
#define LCD_E_F  2
#define LCD_RW_F 1
#define LCD_RS_F 0

#define MAX_INT 9

typedef enum {C_OFF, C_ON} Cursor_Mode;

extern void lcd_init(Cursor_Mode mode);
extern void lcd_clear();
extern void lcd_home();
extern void lcd_reset();
extern void lcd_row0();
extern void lcd_row1();
extern void lcd_cmd(uint8_t command);
extern void lcd_data(uint8_t data);
extern void lcd_set_position(uint8_t row,uint8_t col);
extern int lcd_print_string(const char *pointer);
extern int lcd_print_num(int num);

#endif /* LCD_H */
