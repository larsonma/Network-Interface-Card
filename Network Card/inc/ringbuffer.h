/*
 * ringbuffer.h
 *
 *  Created on: Jan 30, 2017
 *      Author: Mitchell Larson
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

#define BUF_SIZE 50
#define BACK_SPACE 127

typedef struct {
	unsigned int put;
	unsigned int get;
	uint8_t buffer[BUF_SIZE];
} RingBuffer;

extern void put(volatile RingBuffer* buffer, uint8_t element);
extern uint8_t get(volatile RingBuffer* buffer);
extern int hasSpace(volatile RingBuffer* buffer);
extern int hasElement(volatile RingBuffer* buffer);

#endif	/*RINGBUFFER_H*/
