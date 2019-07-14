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

static bool_t chg4Hex2Dec ( uint8_t *ch, uint16_t *data );
static bool_t chg2Hex2Dec ( uint8_t *ch, uint8_t *data );

/**
\brief ４文字のHEXを数値に変換する

\param	*ch	変換する文字列の格納アドレス
\retval	*data	変換後の数値
\note なし
**/
static bool_t chg4Hex2Dec ( uint8_t *ch, uint16_t *data ) {
	uint8_t i;
	bool_t ret = TRUE;

	*data = 0;	
	for ( i = 0 ; i < 4 ; i++ ) {
		if ( ( *ch >= '0' ) && ( *ch <= '9' ) ) {
			*data = *data * 16 + *ch - '0';
		} else if ( ( *ch >= 'a' ) && ( *ch <= 'f' ) ) {
			*data = *data * 16 + *ch - 'a' + 10;
		} else if ( ( *ch >= 'A' ) && ( *ch <= 'F' ) ) {
			*data = *data * 16 + *ch - 'A' + 10;
		} else {
			ret = FALSE;
			break;
		}
		ch++;
	}
	
	return ret;
}

/**
\brief ２文字のHEXを数値に変換する

\param	*ch	変換する文字列の格納アドレス
\retval	*data	変換後の数値
\note なし
**/
static bool_t chg2Hex2Dec ( uint8_t *ch, uint8_t *data ) {
	uint8_t i;
	bool_t ret = TRUE;
	
	for ( i = 0 ; i < 2 ; i++ ) {
		if ( ( *ch >= '0' ) && ( *ch <= '9' ) ) {
			*data = *data * 16 + *ch - '0';
		} else if ( ( *ch >= 'a' ) && ( *ch <= 'f' ) ) {
			*data = *data * 16 + *ch - 'a' + 10;
		} else if ( ( *ch >= 'A' ) && ( *ch <= 'F' ) ) {
			*data = *data * 16 + *ch - 'A' + 10;
		} else {
			ret = FALSE;
			break;
		}
		ch++;
	}
	
	return ret;
}

/**
\brief 1バイトのデータを2文字の16進数に置き換える

\param	data	変換する対象の1バイトデータ
\retval	ret	変換後の文字列
\note なし
**/
static uint16_t chgDec2Hex ( uint8_t data ) {
	uint8_t tmp1,tmp2;
	uint16_t ret;
	
	tmp1 = data & 0x0f;
	tmp2 = ( data >> 4 ) & 0x0f;
	
	if ( tmp1 < 0x0a ) { tmp1 = tmp1 + '0'; }
	else { tmp1 = ( tmp1 - 10 ) + 'A'; }
	
	if ( tmp2 < 0x0a ) { tmp2 = tmp2 + '0'; }
	else { tmp2 = ( tmp2 - 10 ) + 'A'; }
	
	/* retにASCII文字列２文字分が格納される */
	ret = ( tmp2 << 8 ) + tmp1; 
	
	return ret;
}

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

static uint8_t outDatas[ 8 + (4* USED_ADC_CH ) ];
void outputResult ( hPipe_t id, uint16_t *data ){
	uint16_t tmp;
	uint8_t i;
	outDatas[0] = 'A';
	outDatas[1] = 'A';
	outDatas[2] = 'A';
	outDatas[3] = 'A';

	/*  */
	tmp = chgDec2Hex ( (uint8_t)( sizeof(outDatas) & 0x00ff ) );
	outDatas[4] = (uint8_t)('0');
	outDatas[5] = (uint8_t)('0');
	outDatas[6] = (uint8_t)(( tmp >> 8 ) & 0x00ff);
	outDatas[7] = (uint8_t)( tmp & 0x00ff );

	/* ADCで取得するチャンネルは2ch分、14chと15ch */
	for ( i = 0 ; i < USED_ADC_CH ; i++ ) {
		tmp = chgDec2Hex ( (uint8_t)(  ( data[i] >> 8 ) & 0x00ff ) );
		outDatas[i*4 + 8] = (uint8_t)( ( tmp>>8 ) & 0x00ff );
		outDatas[i*4 + 9] = (uint8_t)( tmp & 0x00ff );

		tmp = chgDec2Hex ( (uint8_t)( data[i] & 0x00ff ) );
		outDatas[i*4 +10] = (uint8_t)( ( tmp>>8 ) & 0x00ff );
		outDatas[i*4 +11] = (uint8_t)( tmp & 0x00ff );
	}

	setTxDataToPipe ( (hPipe_t)0, id, outDatas , sizeof( outDatas ) );
	setTxDataToPipe ( (hPipe_t)0, id, (uint8_t *)"\n" , 1 );
	reqTxPipe ( (hPipe_t)0, id, sizeof( outDatas ) + 1);
}

