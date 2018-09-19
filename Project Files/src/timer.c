#include "timer.h"

/*
 *	Delay the processor by t_ms by
 *	polling the Systick timer.
 *	inputs:
 *			t_ms - number of miliseconds to delay
 *	outputs:
 *			none
*/
void delay_ms(uint32_t t_ms){
	//ensure counter is off
	*(STK_CTRL) &= ~(1<<STK_ENABLE_F);
	//t_ms time 
	for(uint32_t i=0; i<t_ms; i++){
		
	
		//load 16000 into STK_LOAD
		*(STK_LOAD) = 16000;//1ms
		
		//turn on counter
		*(STK_CTRL) |= ((1<<STK_ENABLE_F) | (1<<STK_CLKSOURCE_F));
		
		//wait for flag to go to 16
		while((*STK_CTRL & (1<<STK_CNTFLAG_F)) != (1<<STK_CNTFLAG_F)){}
		
		//turn off counter
		*(STK_CTRL) &= ~(1<<STK_ENABLE_F);
	}
}

/*
 *	Delay the processor by t_us by
 *	polling the Systick timer.
 *	inputs:
 *			t_us - number of miliseconds to delay
 *	outputs:
 *			none
*/
void delay_us(uint32_t t_us){
	//ensure counter is off
	*(STK_CTRL) &= ~(1<<STK_ENABLE_F);
	//t_ms time 
	for(uint32_t i=0; i<t_us; i++){
		
	
		//load 16 into STK_LOAD
		*(STK_LOAD) = 16;//1us
		
		//turn on counter
		*(STK_CTRL) |= ((1<<STK_ENABLE_F) | (1<<STK_CLKSOURCE_F));
		
		//wait for flag to go to 16
		while((*STK_CTRL & (1<<STK_CNTFLAG_F)) != (1<<STK_CNTFLAG_F)){}
		
		//turn off counter
		*(STK_CTRL) &= ~(1<<STK_ENABLE_F);
	}
}
