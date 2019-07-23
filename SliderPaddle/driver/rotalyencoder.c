#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "commonDriver.h"
#include "gpioDriver.h"

#include "timer.h"

static uint8_t rotalyCount = 0;

ISR(PCINT0_vect){
	/* uartRXint is function pointer. */
	isrRotary();
}



/**
\brief Controller gpio_int.

#4 GPIOの割り込みにより、ロータリーエンコーダーの回転方向演算開始 \n
割り込み発生の直後（数usec）のGPIOの状態を取得する。

\param		なし
\retval	なし
\note なし
**/
void isrRotary ( void ) {
	uint8_t val;
	val = getGPIO ( IOID_PORT_B );
	if ( ( val & BIT6_VAL ) != 0 )
	{
		val = getGPIO ( IOID_PORT_B );
		/* 割り込み発生後数十us後のもう一方のIO状態から回転方向を察知 */
		if ( ( val & BIT5_VAL ) != 0 ) {
			rotalyCount++;
		} else {
			rotalyCount--;
		}
	}
}

void initRotary ( void  )
{
	/* PB6の入力割り込み許可 */
	PCICR  = PCICR  | ( 1 << PCIE0 );
	PCMSK0 = PCMSK0 | ( 1 << PCINT6 );
}

int8_t getRotaryCount ( void ) {
	int8_t tmp;
	tmp = rotalyCount;
	rotalyCount = 0;
	return tmp;
}


