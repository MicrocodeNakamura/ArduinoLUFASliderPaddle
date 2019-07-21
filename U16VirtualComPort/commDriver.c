#include "stdio.h"
#include "commDriver.h"
#include "typedef.h"

#include "ringBuffer.h"
// pipe controller
static pipeMainControll_t pipeMain;
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

/* データ送信API関数 */
uint8_t stdTxFunc ( hPipe_t id, hPipe_t senderid, uint8_t *data, uint16_t size ) {
	if ( pipeMain.pipe[id].stdTxFunc != NULL ) {
		/* Send data to driver. */
		return pipeMain.pipe[id].stdRxFunc( id, senderid, data, size );
	}
}

/* pipeAPIで受信したデータの標準処理ルーチン。 ポインタ関数を変更することで
   オーバーライド可能 */
uint8_t stdRxFunc( hPipe_t id, hPipe_t senderid, uint8_t *data, uint16_t size) {
	/* 指定されたpipeidのリングバッファにデータを格納する */
	if ( id >= PIPE_MAX ) {
		return /* INVALID_ID */ -1;
	}
	if ( pipeMain.isUse[id] != TRUE ) {
		return /* NOTUSED */ -1;
	}
	
	/* Send data to application. */
	return pipeMain.pipe[id].stdTxFunc( id, senderid, data, size );
}


/*
  Pipe構造体とバッファをひも付けてIDを取得する。
  APIはIPを使ってアクセスする。 
  取得したPIPE IDをドライバAPIの初期化処理に渡すことで、ハードウェアチャネルを論理チャネルに変換する
*/
hPipe_t openDataPipe( uint8_t *buf, cbFunc_t stdRxFunc, cbFunc_t stdTxFunc ) {
	uint8_t id;
	uint8_t ret = 0xff;
	for ( id = 0 ; id < PIPE_MAX ; id++ ) {
		if ( pipeMain.isUse[id] == FALSE ) {
			break;
		}
	}

	if ( id != PIPE_MAX ) {
		pipeMain.isUse[id] = TRUE;
		pipeMain.pipe[id].pBuf = buf;
		pipeMain.pipe[id].pAppData = NULL;
		pipeMain.pipe[id].streamType = 0xffff;        // TBD
		pipeMain.pipe[id].streamID = 0xff;  // TBD
		/*  */
		pipeMain.pipe[id].stdRxFunc = stdRxFunc;  /* 標準受信コールバック関数 */
		pipeMain.pipe[id].stdTxFunc = stdTxFunc;  /* 標準送信コールバック関数 */

		initRingBuffer( &(pipeMain.pipe[id].rx_ring), pipeBuffer[id], PIPE_BUFFER_SIZE );
		initRingBuffer( &(pipeMain.pipe[id].tx_ring), pipeBuffer[id], PIPE_BUFFER_SIZE );
		ret = id;
	}
	return id;
}

/* 指定されたPIPE IDのデータバッファ先頭領域を取得する */

#if 0
/* pipe固有のデータバッファアドレス。　割り込みハンドラがバッファを参照するようなケースで使用する */
uint8_t *getPipeDataBuffer ( hPipe_t id ) {
	uint8_t *ret = NULL;
	if ( id < PIPE_MAX ) {
		if ( pipeMain.isUse[id] == TRUE ) {
			ret = pipeMain.pipe[id].pBuf;
		}
	}

	return ret;
}
#endif

#if 0
/* 指定されたPIPE IDのリングバッファ管理情報のアドレスを返す */

/* 受信ドライバ割り込み処理が受信したデータをリングバッファに格納するようなケースで使用する */
ringBuffer_t *getRingBufferAddr ( hPipe_t id ) {
	ringBuffer_t *ret = NULL;
	if ( id < PIPE_MAX ) {
		if ( pipeMain.isUse[id] == TRUE ) {
			ret = &(pipeMain.pipe[id].ring);
		}
	}

	return ret;
} 
#endif

/* 送信アプリケーション側が呼び出すAPI */
uint16_t setTxDataToPipe ( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	pipeMain.pipe[id].stdTxFunc( prev_id, id, buf, size );
	return ret;
}

/* 送信アプリケーション側が呼び出すAPI */
uint16_t setRxDataFromPipe ( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	pipeMain.pipe[id].stdRxFunc( prev_id, id, buf, size );
	return ret;
}

uint16_t getTxDataFromPipe ( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

uint16_t getRxDataFromPipe ( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/* 受信アプリケーション側が呼び出すAPI サイズ取得 */
uint16_t getTxDataSizeToPipe ( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].tx_ring) );
	return ret;
}

/* 受信アプリケーション側が呼び出すAPI サイズ取得 */
uint16_t getRxDataSizeFromPipe ( hPipe_t id ) {
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
