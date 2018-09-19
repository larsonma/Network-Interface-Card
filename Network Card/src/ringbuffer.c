/*
 * ringbuffer.c
 *
 *  Created on: Jan 30, 2017
 *      Author: Mitchell Larson
 */
#include "ringbuffer.h"

void put(volatile RingBuffer* buffer, uint8_t element);
uint8_t get(volatile RingBuffer* buffer);
int hasSpace(volatile RingBuffer* buffer);
int hasElement(volatile RingBuffer* buffer);

/**
 * This function adds an element to the buffer. If there
 * is no space in the buffer, it will block until space
 * is available
 * Inputs:
 * 		*buffer - pointer to buffer to add element to
 * 		element - character to add to buffer
 * Outputs:
 * 		none
 */
void put(volatile RingBuffer* buffer, uint8_t element){
		//block until the buffer has space
		while(!hasSpace(buffer)){};

		//add the element to the buffer.
		buffer->buffer[buffer->put]=element;

		//increment the put location in the buffer
		if(((buffer->put)+1)==BUF_SIZE){
			buffer->put=0;
		}else{
			(buffer->put)++;
		}
}

/**
 * This function gets an element from the buffer. The order
 * will follow a FIFO order. This function will block if no
 * elements are available until one is available.
 * Inputs:
 * 		*buffer - pointer to the buffer to retrieve a char from
 * Outputs:
 * 		char - element contained in the buffer.
 */
uint8_t get(volatile RingBuffer* buffer){
	while(!hasElement(buffer)){};
	
	//retrieve element
	uint8_t element = buffer->buffer[buffer->get];
	
	//increment the get location in the buffer
	if(((buffer->get)+1)==BUF_SIZE){
		buffer->get=0;
	}else{
		(buffer->get)++;
	}

	return element;
}

/**
 * This function returns a non-zero number if there is
 * room for an element on the buffer.
 * Inputs:
 * 		*buffer - pointer to buffer to check
 * Outputs:
 * 		int - 0-no room, non-zero-room for an element
 */
int hasSpace(volatile RingBuffer* buffer){
	if(((buffer->put)+1)== BUF_SIZE){	//if put is at the end of the buffer
		if((buffer->get)==0){
			return 0;
		}else{
			return 1;
		}
	}else{
		if(((buffer->put)+1)==buffer->get){
			return 0;
		}else{
			return 1;
		}
	}	
}

/**
 * This function will return a non-zero value if there
 * is at least one element in the buffer.
 * Inputs:
 * 		*buffer - buffer to check
 * Outputs:
 * 		0-no elements, non-zero- at least 1 element.
 */
int hasElement(volatile RingBuffer* buffer){
	if(buffer->put==buffer->get){	//if put is at the end of the buffer
		return 0;
	}else{
		return 1;
	}
}
