#ifndef __COMMDRIVE_H__
#define __COMMDRIVE_H__
#include "stdio.h"
#include "typedef.h"
#include "ringBuffer.h"

/*-------------------------------------------------*/
/* リングバッファ用固定長メモリプール */
#define PIPE_MAX	2
#define PIPE_BUFFER_SIZE	64

#define PIPE_ERROR_ID	0xFF
#define PIPE_INVALID	0xFF

/* データハンドル型 */
typedef uint8_t hPipe_t;

/* データ処理コールバック関数ポインタ型 
   ( 送信先ハンドル、 送信元ハンドル、 データポインタ、 データサイズ */
typedef uint8_t (*cbFunc_t)( hPipe_t, hPipe_t, uint16_t);

/* PIPE_MAXがpipeインスタンスの指定部分に相当する */
typedef struct {
	ringBuffer_t rx_ring;		/* リングバッファ管理構造体 */
	ringBuffer_t tx_ring;		/* リングバッファ管理構造体 */
	uint8_t *pBuf;			/* 送受信データ格納バッファポインタ */

	/* アプリケーション制御用データとして使用する */
	void *pAppData;			/* パイプを扱うアプリケーションが使用する構造体の紐付け用 */
	uint16_t appDataSize;

	/* ドライバの物理チャネル制御用データ */
	uint16_t streamType;	/* 入出力タイプ識別子 */
	uint8_t streamID;		/* 物理チャネル識別子 */

	/* データ受信関数ポインタ。
	   タスク間のデータ受け渡しの場合は、このポインタの関数に
	   メッセージ送信処理を記述し、メッセージ受信処理を別途記述する。 */
	cbFunc_t stdTxFunc;
	cbFunc_t stdRxFunc;
} pipeDataControl_t;

/* pipe制御情報を集約するデータ部分、ハンドルが添え字そのもの。 */
typedef struct {
	uint8_t isUse[ PIPE_MAX ];
	pipeDataControl_t pipe[ PIPE_MAX ];
} pipeMainControll_t;

typedef enum {
	SCIF_PARSE_PREPARE,
	SCIF_PARSE_WAIT_HEADER,
	SCIF_PARSE_WAIT_BODY,
	SCIF_PARSE_COMMAND,
	SCIF_PARSE_MAX,
} scif_parse_status_t;

/* PIPE全体の初期化 */
void initMainPipe ( void );
hPipe_t openDataPipe( cbFunc_t stdRxFunc, cbFunc_t stdTxFunc );
uint8_t reqRxPipe ( hPipe_t senderid, hPipe_t id, uint16_t data );
uint8_t reqTxPipe( hPipe_t senderid, hPipe_t id, uint16_t data );
uint16_t setTxDataToPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size );
uint16_t setRxDataFromPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size );
uint16_t getTxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size );
uint16_t getRxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size );
uint16_t getTxPipeDataSize( hPipe_t id );
uint16_t getRxPipeDataSize( hPipe_t id );
#endif /* __COMMDRIVE_H__ */
