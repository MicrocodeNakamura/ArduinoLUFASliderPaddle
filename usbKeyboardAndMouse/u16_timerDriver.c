/*
 * u16Driver.c
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16_timerDriver.h"
#include "ring_buffer.h"

static InterruptCallbackHandler timerInt = NULL;

/* �^�C�}���荞�݃x�N�^�A�h���X */
ISR(TIMER0_COMPA_vect){
	/* uartRXint is function pointer. */
	if ( timerInt != NULL ){ timerInt(); }
}

void timer_handler( void ) {
	static unsigned time = 0;

	if ( time < PORT_SCAN_TIMING_MS ) {
		time++;
	} else {
		if ( PB4_IN() != 1 ){
			/* �{�^����������1�b������USB�̃L�[�R�[�h�𔭍s */
			if ( KeyEnterStatus == 0 ) {
				/* �񓯊���USB�̃L�[�R�[�h���o�v���𔭍s */
				KeyEnterStatus = 1;
			}
		} else {
			if ( KeyEnterStatus == 1 ) {
				/* �񓯊���USB�̃L�[�R�[�h���o�v���𔭍s */
				KeyEnterStatus = 2;
			}
		}
		time = 0;
	}
}

void init_Timer ( 
	InterruptCallbackHandler handler , 
	 /* you can specify TIMER_CLKDIV_[n] in deviceint.h */
	unsigned char TimerUnitMode ,
	unsigned char time ){

	unsigned char c;

	timerInt = handler;

	c = (unsigned char)(0x02);
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(TCCR0A)),"a"(c) );

	c = TimerUnitMode;
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(TCCR0B)),"a"(c) );

	c = time;
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(OCR0A)),"a"(c) );

	TIMSK0 = (unsigned char)(0x02);

	c = (unsigned char)(0x02);
	asm volatile( "out %0,%1"::"I"(_SFR_IO_ADDR(TIFR0)),"a"(c) );
}
/* void TimerHandler ( void ){} */
