#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "commonDriver.h"
#include "gpioDriver.h"

#include "mainApp.h"
#include "timer.h"

static uint8_t rotalyCount[PLAYERS];

void isrRotary ( uint8_t ch, uint8_t val );

/**
\brief Controller gpio_int.

#4 GPIOの割り込みにより、ロータリーエンコーダーの回転方向演算開始 \n
割り込み発生の直後（数usec）のGPIOの状態を取得する。

\param		なし
\retval	なし
\note なし
**/

void isrRotary ( uint8_t ch, uint8_t val ) {
	/* 指定されたチャネルに対応するデータのH、Lで回転方向のカウントを積算する。 */
	if ( val != 0 )
	{
		rotalyCount[ch]--;

	} else {
		rotalyCount[ch]++;
	}
}

/*
Port mapping - MEGA2560 ボード側回路図に反映(Todo)

                Rot1(INT)          Rot2               Button
Player 1        PD0                PA0                PC0
Player 2        PD1                PA1                PC1
Player 3        PD2                PA2                PC2
Player 4        PD3                PA3                PC3
Player 5        PE4                PA4                PC4
Player 6        PE5                PA5                PC5
*/

/*
PD0-3  Communication 21-18
PE4-5  PWML 2-3
PA0-5  DIGITAL 22-27
PC0-5  DIGITAL 37-32
*/

/* 回転方向を検知する割り込み処理。 IOポートの読み出し処理が数usのウェイトを兼ねwている。 */
ISR(INT0_vect){ uint8_t val = PINA & BIT0_VAL; isrRotary ( 0, val ); }
ISR(INT1_vect){ uint8_t val = PINA & BIT1_VAL; isrRotary ( 1, val ); }
ISR(INT2_vect){ uint8_t val = PINA & BIT2_VAL; isrRotary ( 2, val ); }
ISR(INT3_vect){ uint8_t val = PINA & BIT3_VAL; isrRotary ( 3, val ); }
ISR(INT4_vect){ uint8_t val = PINA & BIT4_VAL; isrRotary ( 4, val ); }
ISR(INT5_vect){ uint8_t val = PINA & BIT5_VAL; isrRotary ( 5, val ); }

void initRotary ( void  )
{
	uint8_t i;
	
	for ( i = 0 ; i < sizeof ( rotalyCount) ; i++ ) {
		rotalyCount[i] = 0;
	}
	
	DDRA = 0xC0;
	DDRC = 0xC0;
	DDRD = 0xF0;
	DDRE = 0xCF;
	
	/* PD0（INT0）～PD3（INT3） を割り込み端子に設定 */
	EICRA = 0xFF;
	/* PE4（INT4）～PE5（INT5） を割り込み端子に設定 */
	EICRB = 0x0F;
	
	/* 外部割込み許可フラグのセット */
	EIMSK = 0x3F;
}

int8_t getRotaryCount ( uint8_t ch ) {
	int8_t tmp;

	disableInterrupt();
	tmp = rotalyCount[ch];
	rotalyCount[ch] = 0;
	enableInterrupt();

	return tmp;
}


