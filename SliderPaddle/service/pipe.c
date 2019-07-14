/**
\brief 通信インターフェース接続サービス
*/

#include "stdio.h"
#include "commonDriver.h"
#include "typedef.h"

#include "ringBuffer.h"
#include "pipe.h"

// pipe controller
static pipeMainControll_t pipeMain;
static uint8_t pipeBuffer[PIPE_MAX][PIPE_BUFFER_SIZE];

/**
\brief pipe機能の初期化

pipe管理構造体の初期化
\retval	なし
\note なし
**/
void initMainPipe ( void ) {
	uint8_t i;
	for ( i = 0 ; i < PIPE_MAX ; i++ ){
		pipeMain.isUse[i] = FALSE;
	}
}

/**
\brief function brief

Pipe構造体とバッファをひも付けてIDを取得する\n
APIはIPを使ってアクセスする\n
取得したPIPE IDをドライバAPIの初期化処理に渡すことで、ハードウェアチャネルを論理チャネルに変換する
\param	stdRxFunc	受信契機コールバック関数ポインタ
\param	stdTxFunc	送信処理呼び出し関数ポインタ
\retval	pipe ID
\note なし
**/
hPipe_t openDataPipe( cbFunc_t stdRxFunc, cbFunc_t stdTxFunc ) {
	uint8_t id;

	for ( id = 0 ; id < PIPE_MAX ; id++ ) {
		if ( pipeMain.isUse[id] == FALSE ) {
			break;
		}
	}

	if ( id != PIPE_MAX ) {
		pipeMain.isUse[id] = TRUE;
		pipeMain.pipe[id].streamType = 0xffff;        // TBD
		pipeMain.pipe[id].streamID = 0xff;  // TBD
		/*  */
		pipeMain.pipe[id].stdRxFunc = stdRxFunc;  /* 標準受信コールバック関数 */
		pipeMain.pipe[id].stdTxFunc = stdTxFunc;  /* 標準送信コールバック関数 */

		initRingBuffer( &(pipeMain.pipe[id].rx_ring), pipeBuffer[id], PIPE_BUFFER_SIZE );
		initRingBuffer( &(pipeMain.pipe[id].tx_ring), pipeBuffer[id+1], PIPE_BUFFER_SIZE );
	} else {
		id = PIPE_ERROR_ID;
	}
	return id;
}

/**
\brief データ受信処理要求発行（App <- Driver)

データ送受信要求は、データの処理トリガの発行関数を呼び出す\n
たとえば、受信であればハンドラからこの関数を呼び出すことで、\n
BottomHalf処理契機を作ることができる

\param	senderid	呼び出し元pipeID
\param	id	呼び出し先pipeID
\param	size	受信データサイズ
\retval	正常終了なら0を返す
\note なし
**/
uint8_t reqRxPipe ( hPipe_t senderid, hPipe_t id, uint16_t size ) {
	if ( ( id >= PIPE_MAX ) || ( pipeMain.isUse[id] != TRUE ) ){
		return PIPE_ERROR_ID;
	}

	if ( pipeMain.pipe[id].stdRxFunc != NULL ) {
		/* Send data to driver. */
		return pipeMain.pipe[id].stdRxFunc( senderid, id, size );
	}
	/* pipe handle is not found. */
	return PIPE_ERROR_ID;
}

/**
\brief データ送信処理要求発行（App -> Driver)

アプリケーションがデータ送信関数を呼び出すための関数
\param	senderid	呼び出し元 pipe ID
\param	id	呼び出し先 pipe ID
\param	size	受信データサイズ
\retval	正常終了なら0を返す
\note なし
**/
uint8_t reqTxPipe( hPipe_t senderid, hPipe_t id, uint16_t size) {
	if ( ( id >= PIPE_MAX ) || ( pipeMain.isUse[id] != TRUE ) ){
		return /* INVALID_ID */ -1;
	}
	/* Send data to application. */
	return pipeMain.pipe[id].stdTxFunc( senderid, id, size );
}

/**
\brief 送信データ（App -> Driver) をリングバッファにセット

アプリケーションがデータを送信するデータをpipeのリングバッファにセットする
\param	prev_id 呼び出し元 pipe ID
\param	id	呼び出し先 pipe ID
\param	buf	送信するデータの先頭
\param	size	送信するデータのサイズ
\retval	セットできたデータのサイズ
\note なし
**/
uint16_t setTxDataToPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = setRingBuffers ( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

/**
\brief 受信データ（App <- Driver) をリングバッファにセット

ドライバがアプリケーションに通知する受信データをpipeのリングバッファにセットする
\param	prev_id	呼び出し元 pipe ID
\param	id	呼び出し先 pipe ID
\param	buf	送信するデータの先頭
\param	size	受信したデータのサイズ
\retval	セットできたデータのサイズ
\note なし
**/
uint16_t setRxDataFromPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = setRingBuffers ( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/**
\brief 送信データ（App -> Driver) をリングバッファから取得

アプリケーションが送信したデータをドライバがpipeのリングバッファから\n
取得するための関数

\param	id	参照する pipe ID
\param	buf	取得したデータの格納先
\param	size	取得するデータのサイズ
\retval	取得できたデータサイズ
\note なし
**/
uint16_t getTxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}


/**
\brief 受信データ（App <- Driver) をリングバッファから取得

ドライバが受信したデータをアプリケーションがリングバッファから\n
取得するための関数

\param	id	参照する pipe ID
\param	buf	取得したデータの格納先
\param	size	取得するデータのサイズ
\retval	取得できたデータサイズ
\note なし
**/
uint16_t getRxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/**
\brief 送信リングバッファに格納したデータサイズを参照する

ドライバがHOSTPCにデータ送信する際に、送信データサイズを参照する

\param	id	参照する pipe ID
\retval	格納されているデータサイズ
\note なし
**/
uint16_t getTxPipeDataSize( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].tx_ring) );
	return ret;
}

/**
\brief 受信リングバッファに格納したデータサイズを参照する

アプリケーションがデータ受信する際に、受信データサイズを参照する

\param	id	参照する pipe ID
\retval	格納されているデータサイズ
\note なし
**/
uint16_t getRxPipeDataSize( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].rx_ring) );
	return ret;
}

#if 0
/* callback routine template */
uint8_t rxStdCallback( hPipe_t, hPipe_t, uint8_t *, uint16_t);
uint8_t txStdCallback( hPipe_t, hPipe_t, uint8_t *, uint16_t);

uint8_t rxStdCallback( hPipe_t prev_id, hPipe_t id, uint8_t *buffer, uint16_t size)
{
	return 0;
}

uint8_t txStdCallback( hPipe_t prev_id, hPipe_t id, uint8_t *buffer, uint16_t size)
{
	return 0;
}
#endif
