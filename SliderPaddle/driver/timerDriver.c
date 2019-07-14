/*
 * timerDriver.c
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timerDriver.h"
#include "ringBuffer.h"
#include "typedef.h"

static InterruptCallbackHandler timerInt = NULL;

/**
\brief タイマ割り込みハンドラ TIMER0_COMPA

レジスタで指定された周期で割り込み信号が発生するたびに、ハードウェアから呼び出される

\retval	なし
\note なし
**/
ISR(TIMER0_COMPA_vect){
	/* uartRXint is function pointer. */
	if ( timerInt != NULL ){ timerInt(); }
}

/**
\brief タイマハンドラ

割り込みハンドラから呼び出される。 タイマ割り込み処理はこの関数内部に記述する

\retval	なし
\note なし
**/
static uint16_t timerTime = 0;

void timer_handler( void ) {
	timerTime++;
}

/**
\brief タイマ機能の初期化

割り込みハンドラの関数ポインタや、タイマの周期を設定する

\param	handler	タイマハンドラの関数ポインタ
\param	TimerUnitMode	レジスタに設定するタイマの動作モード（TBD)
\param	time	割り込み発生までの周期カウンタ値(TBD)
\retval	なし
\note なし
**/
void init_Timer ( InterruptCallbackHandler handler ){
	timerInt = handler;
	/* OC0A、OC0B切断 WGM1、WGM0 = [10] */
	TCCR0A = (uint8_t)(0x02);
	/* FOC0A,FOC0B = [00] WGM2 = [0] CS0 2-0 = [011] Clock start and 1/64 pre-scaler. */
	TCCR0B = (uint8_t)(0x03);
	/* 25 count = (1 *10^-3 / (1/1.6*10^6) * 64 ) なぜ8倍しないといけないのかがわからない。 要測定 */
	OCR0A = (uint8_t)(200);
	OCR0B = (uint8_t)(254);
	/* count clear */
	TCNT0 = (uint8_t)(0x00);
	/* Timer interrupt TIMER A enable */
	TIMSK0 = (uint8_t)(0x02);
	/* Timer interrupt request clear */
	TIFR0 = (uint8_t)(0x02);
}

void timerSuspend( void ) {
	/* FOC0A,FOC0B = [00] WGM2 = [0] CS2-0 = [011] Clock disable. */
	TCCR0B = (uint8_t)(0x00);
	TCNT0 = (uint8_t)(0x00);	
}

void timerResume ( void ) {
	/* FOC0A,FOC0B = [00] WGM2 = [0] CS2-0 = [011] Clock start and 1/64 pre-scaler. */
	TCCR0B = (uint8_t)(0x04);	
}

uint16_t timerGetTime( void ) {
	return timerTime;
}

void timerClearTime( void ) {
	timerTime = 0;
}