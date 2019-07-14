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
#include "parser.h"

#include "adcDriver.h"

hPipe_t rxPipe;
hPipe_t txPipe;

static uint8_t sig;
static hPipe_t scifHandle;

/**
\brief applicationの初期化処理


グローバル変数等の初期化を行う（中身なし）
\retval	なし
\note なし
**/
void initApp( void )
{
	/* アプリケーションパラメータの初期化 */

	/* Pipe Serviceの初期化 */
	initMainPipe();
	scifHandle = openDataPipe( rxSignal, txSignal );

	/* SCIFドライバをpipeに登録 */
	init_UART (scifHandle);
	
	/* パーサーの初期化 */
	initParser( scifHandle );
}

/**
\brief アプリケーション処理のメインループ

UART受信ハンドラのデータ受信通知を待ち続けるメインループ\n
UART送信はparserが受け持つためイベントフラグ未使用

UARTからデータを受信すると、parser処理を呼び出す
ループから抜ける契機はない

\retval	なし
\note なし
**/
void ApplicationTask ( bool_t fire )
{
	uint16_t datas[USED_ADC_CH];
	
	if ( fire == TRUE ) {
		(void)getADCValue ( ADC_CH_A, &datas[0] );
		(void)getADCValue ( ADC_CH_B, &datas[1] );

		/* データ送信処理 */
		outputResult ( scifHandle, datas );
	}
	
	/* アプリケーションタスクループ */
	if ( ( sig & RX_SIG ) != 0 ) {
		/* データ受信フラグがセットされていたときの処理 */
		sig &= (uint8_t)~RX_SIG;

		/* パーサー処理の呼び出し
			パーサ内部でデータ送信要求があった場合、パーサが送信シグナルをセットする */
	} else if ( ( sig & TX_SIG ) != 0 ) {
		sig &= (uint8_t)~TX_SIG;
		/* データ送信機能がデータを送信するので、このイベントフラグではなにもしない */
	}
}

/**
\brief 受信コールバック関数 データ受信のシグナルを上げる

UART受信時に呼び出されるコールバック関数から呼び出される\n
メインループのparser処理を呼び出すためのイベントフラグをセットする

\param	prevId	呼び出し元pipe id
\param	id	呼び出し先pipe id
\param	data	使わない
\retval	常に0を返す
\note 割り込みハンドラ内からの呼び出しなので、なるべく早く関数を終了すること
**/
uint8_t rxSignal( hPipe_t prevId, hPipe_t id, uint16_t data)
{
	sig |= RX_SIG;
	return 0;
}

/**
\brief UART データ送信関数

アプリケーションがUART送信データ（応答データ）をセットしたときに呼び出す関数\n
送信リングバッファに格納されている送信データを全てUART上に送信する処理を呼び出す

\param	prevId	呼び出し元pipe id
\param	id	呼び出し先pipe id
\param	data	使わない
\retval	常に0を返す
\note なし
**/
uint8_t txSignal( hPipe_t prevId, hPipe_t id, uint16_t data)
{
	/* TX送信と送信完了割り込みの書き込み処理起動開始 */
	uart_write( scifHandle );
	return 0;
}

