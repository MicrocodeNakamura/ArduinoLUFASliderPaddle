#define GBUF_SIZE 128

void checksize( ringBuffer_t *pr, uint8_t *label );

uint8_t gbuf[GBUF_SIZE];
ringBuffer_t ring;
ringBuffer_t *pr;

void checksize( ringBuffer_t *pr, uint8_t *label ) {
	printf ( "LABEL:%s ", label );
	if ( isRingBufferEmpty(pr) == TRUE ) {
		printf ( "Ring Buffer is empty\n" );
	} else if ( isRingBufferFull(pr) == TRUE ) {
		printf ( "Ring Buffer is full=%03d\n", getRingBufferDataSize(pr) );
	} else {
		printf ( "Ring Buffer has data=%03d\n", getRingBufferDataSize(pr) );
	}
}

void main( void ) {
	uint8_t data,i;
	uint8_t datas[20];
	pr = &ring;

	initRingBuffer( pr, gbuf, GBUF_SIZE );
	checksize ( pr, "1" );
	setRingBuffer( pr, 'a' );
	checksize ( pr, "2" );
	getRingBuffer( pr, &data);
	checksize ( pr, "3" );

	setRingBuffers( pr, "0123456789" , 10 );
	checksize ( pr, "4" );
	getRingBuffers( pr, datas, 10 );
	datas[10] = '\0';
	printf ( "%s\n", datas );
	checksize ( pr, "5" );

	for ( i = 0 ; i < 13 ; i++ ) {
		setRingBuffers( pr, "0123456789" , 10 );
		checksize ( pr, "6+" );
	}
	for ( i = 0 ; i < 12 ; i++ ) {
		getRingBuffers( pr, datas, 10 );
		datas[10] = '\0';
		printf ( "%s\n", datas );
		checksize ( pr, "6-" );
	}
	checksize ( pr, "7" );
	for ( i = 0 ; i < sizeof( datas ); i++ ) { datas[i] = '\0'; }
	getRingBuffers( pr, datas, 10 );
	printf ( "%s\n", datas );
}
