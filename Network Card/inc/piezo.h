/*
 * piezo.h
 *
 *  Created on: Jan 25, 2017
 *      Author: Mitchell Larson
 */

#ifndef PIEZO_H
#define PIEZO_H

//libraries needed
#include <inttypes.h>
#include "gpio.h" 
#include "timer.h"

//Data Structures
typedef enum {
	C, D, E, F, G, A, B, REST
} Letter;

typedef enum {
	FLAT=-1, NATURAL=0, SHARP=1
} Accidental;
 
typedef struct {
	Letter letter;
	Accidental accidental;
	uint8_t octave;
	float duration_ms;
} Note;

typedef struct {
	double frequency;
	float duration_ms;
} Tone;


//macros used
#define STK_ENABLE 0x01
#define STK_TICKINT 0x02
#define STK_CLKSOURCE 0x04
#define STK_COUNTFLAG 0x10000

#define APB1ENR_TIM3_ENABLE 1<<1

#define GPIOB_ODR (volatile uint32_t*) 		0x40020414
#define APB1ENR (volatile uint32_t*) 		0x40023840
#define TIM3_OUTPUT_MODE_TOGGLE 0x07<<4
#define TIM3_OUTPUT_EN 0x01
#define TIM3_ON 0x01

//fucntions
extern void init_piezo();
extern void play_note(const Note *note);
extern void play_tone(const Tone *tone);
extern void play_note_sequence(const Note *notes, uint32_t length);
extern void play_tone_sequence(const Tone *tones, uint32_t length);

#endif /* PIEZO_H */
