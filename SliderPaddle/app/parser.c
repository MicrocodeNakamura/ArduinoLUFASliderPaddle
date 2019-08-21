/*
 * mainApp.c
 *
 * Created: 2018/08/30
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "pipe.h"
#include "scifDriver.h"
#include "mainApp.h"
#include "gpioDriver.h"

int8_t getRotaryCount ( uint8_t ch );

/**
\brief parserの初期化処理

UART受信リングバッファの中身をクリアする

\param	id	pipe idでクリアするデータバッファを指定
\retval	なし？
\note なし
**/
void initParser( hPipe_t id )
{
}

static uint8_t outDatas[ 6 + (2* PLAYERS ) ];

void outputResult ( hPipe_t id ){
	/**
	プリアンプル     2
	データサイズ     4
	ロータリーSW     6 （符号付き、右方向正）
	*/

	uint8_t i;
	uint8_t val;

	/* データサイズ */
	outDatas[0] = 0xaa;
	outDatas[1] = 0xaa;
	outDatas[2] = 0;
	outDatas[3] = 0;
	outDatas[4] = 0;
	outDatas[5] = (uint8_t)( sizeof(outDatas) );

	val = ( PINC & 0x3F );
	
	/* Players は6．  */
	for ( i = 0 ; i < PLAYERS ; i++ ) {
		int8_t tmpkey;

		/* Debug */
		tmpkey = getRotaryCount(i);
		if ( tmpkey != 0 ) {
			volatile uint8_t ix;
			ix = 0;
		}

//		outDatas[6 + 0 + i*2 ] = getRotaryCount(i);
		outDatas[6 + 0 + i*2 ] = (uint8_t)tmpkey;
		outDatas[6 + 1 + i*2 ] = ( val >> i ) & 0x01;
	}

	setTxDataToPipe ( (hPipe_t)0, id, outDatas , sizeof( outDatas ) );
	reqTxPipe ( (hPipe_t)0, id, sizeof( outDatas ) );
}

