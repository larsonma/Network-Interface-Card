/*
 * piezo.c
 *
 *  Created on: Jan 25, 2017
 *      Author: Mitchell Larson
 */
 
#include <inttypes.h>
#include <math.h>
#include "gpio.h"
#include "timer.h"
#include "piezo.h"

typedef struct{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SMCR;
	uint32_t DIER;
	uint32_t SR;
	uint32_t EGR;
	uint32_t CCMR1;
	uint32_t CCM2R;
	uint32_t CCER;
	uint32_t CNT;
	uint32_t PSC;
	uint32_t ARR;
	uint32_t BLANK;
	uint32_t CCR1;
	uint32_t CCR2;
	uint32_t CCR3;
	uint32_t CCR4;
	uint32_t DCR;
	uint32_t DMAR;
} TIM3;

#define PRESCALAR 16

static volatile TIM3 *tim3 = (TIM3 *) 0x40000400;

void init_piezo();
void play_note(const Note *note);
void play_tone(const Tone *tone);
void play_note_sequence(const Note *notes, uint32_t length);
void play_tone_sequence(const Tone *tones, uint32_t length);
static Tone convert_note(const Note *note);

/**
 * This function will initialize the piezo buzzer in order to play
 * frequencies. In order to do this, GPIOB pin 4 must be set up
 * and TIM3 must be set to PWM mode. A prescalar must also be set
 * to 16 on TIM3 in order to allow lower frequencies to be played.
 * Inputs:
 * 		none
 * Outputs:
 * 		none
 */
void init_piezo(){
	enable_clock('B');
	set_pin_mode('B',4,ALTFUNC);
	
	//Use TIM3 to set the frequency for the timer
		//enable clock for TIM3 on APB1ENR bit 1
		//enable alt func for PB4
		//select output mode toggle
		//compare output enable
	*(APB1ENR) |= APB1ENR_TIM3_ENABLE;
	set_alt_func('B',4,2);
	tim3->CCMR1 &= ~(0x7<<4);
	tim3->CCMR1 |= TIM3_OUTPUT_MODE_TOGGLE;
	tim3->CCER  |= TIM3_OUTPUT_EN;
	tim3->PSC = PRESCALAR;
}

/**
 * This function will play a note specified by the caller of the
 * function. In reality, the note will converted to a tone, and
 * the function to play a tone will be called.
 * Inputs:
 * 		*note - pointer to the note to be played
 * Outputs:
 * 		none
 */
void play_note(const Note *note){
	Tone temp = convert_note(note);
	play_tone(&temp);
}

/**
 * This function will play a tone by causing the buzzer to vibrate
 * at the specified frequency for the specified amount of time.
 * To set the frequency, the ARR register must be set, with a
 * duty cycle of half the frequency set in the CCR1 register.
 * CNT is reset before playing the frequency to avoid a pause
 * in sound when the CNT register resets to 0.
 * Inputs:
 * 		*tone - pointer to the tone to be played
 * Outputs:
 * 		none
 */
void play_tone(const Tone *tone){
	tim3->CNT = 0;
	uint32_t freq = ((1/(tone->frequency))*(1000000));
	tim3->ARR = freq;
	tim3->CCR1 = (int)(freq/2);			//set duty cycle in he CCR1
	tim3->CR1 |= TIM3_ON;				//enable clock
	delay_ms(tone->duration_ms);		//delay
	tim3->CR1 &= ~(TIM3_ON);			//turn timer off
}

/**
 * This is a helper function that performs the task of translating
 * a Note type to a Tone type. This function is based on
 * determining the number of steps the note is from note C4. Then
 * the frequency of C4 is multiplied by 2 to the power of steps/12.
 * To determine the number of steps a note is from C4, a piecewise
 * function is used since F sharp/E flat does not exist.
 * Inputs:
 * 		*note - pointer to a note to be converted
 * Outputs:
 * 		Tone - the converted note in a Tone representation
 */
static Tone convert_note(const Note *note){
	//using a base of C4=261.63
	double freq;
	if(note->letter!=REST){
		int octaveOffset = ((note->octave)-4);
		int letterOffset = ((note->letter) - C);
		double steps;
		if(letterOffset<=2){
			steps = ((letterOffset*2)+(12*octaveOffset))+(note->accidental);
		}else{
			steps = (((letterOffset*2)-1)+(12*octaveOffset))+(note->accidental);
		}
		freq = 261.63*pow((double)2,(steps/12));
	}else{
		freq = 0;
	}

	Tone tone = {freq,note->duration_ms};
	return tone;
}

/**
 * This function accepts a array of tones and plays the sequence of
 * tones with a delay of 5 ms between tones to keep them distinct.
 * Inputs:
 * 		*tones - a pointer to an array of Tones
 * Outputs:
 * 		none
 */
void play_tone_sequence(const Tone *tones, uint32_t length){
	for(int i=0; i<length; i++){
		play_tone(&tones[i]);
		delay_ms(5);
	}
}

/**
 * This function accepts a array of notes and plays the sequence of
 * notes with a delay of 5 ms between notes to keep them distinct.
 * Inputs:
 * 		*notes - a pointer to an array of Notes
 * Outputs:
 * 		none
 */
void play_note_sequence(const Note *notes, uint32_t length){
	for(int i=0; i<length; i++){
		play_note(&notes[i]);
		delay_ms(5);
	}
}
