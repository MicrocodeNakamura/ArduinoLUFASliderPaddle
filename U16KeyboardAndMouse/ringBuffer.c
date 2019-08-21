#include "stdio.h"
#include "typedef.h"
#include "ringBuffer.h"

void initRingBuffer( ringBuffer_t *ring, uint8_t *buffer, uint16_t size ){
	ring->buf = buffer;
	ring->rp = 0;
	ring->wp = 0;
	ring->size = size;
}

bool_t setRingBuffer( ringBuffer_t *ring, uint8_t data )
{
	bool_t ret = TRUE;
	
	// disable interrupt
	if ( ((ring->wp+1) % ring->size) != ring->rp ){
		ring->buf[ring->wp] = data;
		ring->wp = (ring->wp + 1) % ring->size;
		ret = 1;
	}
	// enable interrupt
	return ret;
}

bool_t getRingBuffer( ringBuffer_t *ring, uint8_t *data )
{
	bool_t ret = FALSE;
	
	// disable interrupt
	if ( (ring->rp != ring->wp ) ){
		*data = ring->buf[ring->rp];
		ring->rp = (ring->rp + 1) % ring->size;
		ret = TRUE;
	}
	// enable interrupt

	return ret;
}

uint16_t setRingBuffers( ringBuffer_t *ring, uint8_t *data, uint16_t size )
{
	uint16_t i;
	uint8_t *pt;

	disableInterrupt();
	pt = data;
	
	for ( i = 0 ; i < size ; i++ ) {
		if ( setRingBuffer( ring, *pt ) == FALSE ) {
			break;
		}
		pt++;
	}
	enableInterrupt();
	return i;
}

uint16_t getRingBuffers( ringBuffer_t *ring, uint8_t *data, uint16_t size )
{
	uint16_t i;
	uint8_t *pt;

	disableInterrupt();
	pt = data;
	
	for ( i = 0 ; i < size ; i++ ) {
		if ( getRingBuffer( ring, pt ) == 0 ) {
			break;
		}
		pt++;
	}
	enableInterrupt();
	return i;
}

uint16_t getRingBufferDataSize( ringBuffer_t *ring ) {
	uint16_t size;
	if ( ring->wp < ring->rp ) {
		size = ( ring->wp + ring->size ) - ring->rp;
	} else {
		size = ring->wp - ring->rp;
	}
	return size;
}

bool_t isRingBufferEmpty( ringBuffer_t *ring )
{
	bool_t ret = FALSE;
	if ( ring->wp == ring->rp ) {
		ret = TRUE;
	}
	return ret;
}

bool_t isRingBufferFull( ringBuffer_t *ring )
{
	bool_t ret = FALSE;
	if ( ((ring->wp+1) % ring->size) == ring->rp ) {
		ret = TRUE;
	}
	return ret;
}

#ifdef FEATURE_TESTCODE
#include "ringBuffer.f"
#endif /* FEATURE_TESTCODE */

