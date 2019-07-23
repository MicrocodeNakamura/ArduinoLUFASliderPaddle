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
#include "u16Driver.h"

static InterruptCallbackHandler timerInt = NULL;

/* �^�C�}���荞�݃x�N�^�A�h���X */
ISR(TIMER0_COMPA_vect){
	/* uartRXint is function pointer. */
	if ( timerInt != NULL ){ timerInt(); }
}

void timer_handler( void ) {
	static unsigned time = 0;

	/* Timer �����֐��̌Ăяo�����L�q����B �A�v���P�[�V�������W�b�N�������Ȃ��B*/
}

/* ������TCCR0A�AOCR0A���W�X�^�̐ݒ�l����͂���B�@�l�̐ݒ���@�̋L�q��TBD�B�@�T���v����1ms�P�ʁB */
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
