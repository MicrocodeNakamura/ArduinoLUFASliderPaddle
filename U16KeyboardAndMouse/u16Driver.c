/*
 * u16Driver.c
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16Driver.h"
#include "ring_buffer.h"

/* debug */
uint8_t debugFlag = 0;

uint16_t KeyEnterStatus = 0;
/* �o�b�t�@�I�[�o�[�t���[�����f�[�^�̗ʂ��J�E���g�A�b�v����� */
uint8_t readBufferOverflow = 0;

/* ���C���^�X�N�̃L���X�C�b�` */
uint8_t breaker = 0;

/* Interrupt vector registration. ----- */
ISR(PCINT0_vect){
//	uart_write ( 'b' ); // send echo back
}

/*
  for debug 
*/
/* HeartBeat�p��LED����֐� */
void debugCheckLedBlinker( void )
{
	static unsigned char led = 0;
	unsigned char reg;
	reg = DEBUG_LED_PORT;
	if ( led == 0 ){
		led = 1;
		reg |= 1 <<(DEBUG_LED_BIT);
		} else {
		reg &= ~(1 <<(DEBUG_LED_BIT));
		led = 0;
	}
	PORTB = reg;
}

void init_GPIO( void )
{
	PIND = 0xff;
	DDRD = 0x80;
}

/* USB Device ��Enable���菈�� */
bool isUSBDeviceEnable( void )
{
	volatile uint8_t data;
	/* PB6 �� H�Ȃ�USB Device Enable */
	data = PB6_IN();
	if ( data != 1 )
	{
		return 0;
	}
	return 1;
}


void init_u16Driver(void)
{
	/* GPIO initilaize */
	init_GPIO();
	
#ifdef MAIN_TASK_KILL_SWITCH_ENABLE
	breaker = 1;
#endif /* MAIN_TASK_KILL_SWITCH_ENABLE */		
	/* enableInterrupt(); */
}
