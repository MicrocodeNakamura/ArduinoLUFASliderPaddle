#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__
#include "stdio.h"
#include "typedef.h"

typedef struct {
	uint8_t *buf;
	uint16_t rp;
	uint16_t wp;
	uint16_t size;
} ringBuffer_t;

// function prototype
void initRingBuffer( ringBuffer_t *ring, uint8_t *buffer, uint16_t size );
bool_t   setRingBuffer( ringBuffer_t *ring, uint8_t data );
bool_t   getRingBuffer( ringBuffer_t *ring , uint8_t *data);
uint16_t setRingBuffers( ringBuffer_t *ring, uint8_t *data, uint16_t size );
uint16_t getRingBuffers( ringBuffer_t *ring, uint8_t *data, uint16_t size );
uint16_t getRingBufferDataSize( ringBuffer_t *ring );

bool_t isRingBufferEmpty( ringBuffer_t *ring );
bool_t isRingBufferFull( ringBuffer_t *ring );
#endif /* __RINGBUFFER_H__ */
