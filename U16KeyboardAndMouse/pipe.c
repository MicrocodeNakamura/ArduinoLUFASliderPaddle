/**
\brief 通信インターフェース接続サービス
*/

#include "stdio.h"
#include "u16_commonDriver.h"
#include "typedef.h"

#include "ringBuffer.h"
#include "pipe.h"

// pipe controller
/* static */pipeMainControll_t pipeMain;
static uint8_t pipeBuffer[PIPE_MAX][PIPE_BUFFER_SIZE];

void testcb( hPipe_t previd, hPipe_t id, uint8_t *buf, uint16_t size ) {
	/* dummy */
}

/*
recv 受信バッファ管理構造体
*/
void initMainPipe ( void ) {
	uint8_t i;
	for ( i = 0 ; i < PIPE_MAX ; i++ ){
		pipeMain.isUse[i] = FALSE;
	}
}

/*
  Pipe構造体とバッファをひも付けてIDを取得する。
  APIはIPを使ってアクセスする。 
  取得したPIPE IDをドライバAPIの初期化処理に渡すことで、ハードウェアチャネルを論理チャネルに変換する
*/
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

/* データ送受信要求は、データの処理トリガの発行関数を呼び出す。
   たとえば、受信であればハンドラからこの関数を呼び出すことで、BottomHalf処理契機を作ることができる */
/* データ受信要求（App <- Driver) */
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

/* データ送信要求（App -> Driver) */
uint8_t reqTxPipe( hPipe_t senderid, hPipe_t id, uint16_t size) {
	if ( ( id >= PIPE_MAX ) || ( pipeMain.isUse[id] != TRUE ) ){
		return /* INVALID_ID */ -1;
	}
	/* Send data to application. */
	return pipeMain.pipe[id].stdTxFunc( senderid, id, size );
}

/* 送信データ（App -> Driver) をリングバッファにセット */
uint16_t setTxDataToPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = setRingBuffers ( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

/* 受信データ（App <- Driver) をリングバッファにセット */
uint16_t setRxDataFromPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = setRingBuffers ( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/* 送信データ（App -> Driver) をリングバッファから取得 */
uint16_t getTxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

/* 受信データ（App <- Driver) をリングバッファから取得 */
uint16_t getRxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/* 受信アプリケーション側が呼び出すAPI サイズ取得 */
uint16_t getTxPipeDataSize( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].tx_ring) );
	return ret;
}

/* 受信アプリケーション側が呼び出すAPI サイズ取得 */
uint16_t getRxPipeDataSize( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].rx_ring) );
	return ret;
}

bool_t isRxPipeIsEmpty( hPipe_t id ) {
	if ( isRingBufferEmpty( &(pipeMain.pipe[id].rx_ring) ) ){
		return TRUE;
	} else {
		return FALSE;		
	}
}

bool_t isTxPipeIsEmpty( hPipe_t id ) {
	if ( isRingBufferEmpty( &(pipeMain.pipe[id].tx_ring) ) ){
		return TRUE;
		} else {
		return FALSE;
	}
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
