/*
 * mainApp.h
 *
 * Created: 2018/08/30
 * Author : ya_nakamura
 */ 
#ifndef __PARSER_H__
#define __PARSER_H__	/**< \brief ここにコメントを記載する. */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

typedef enum {
	DATA_PARSER_NO_INIT, 			/* 未初期化状態。 このステート以外は
									   すべて初期化済み状態 */
	DATA_PARSER_RECEIVING_SIZE, 	/* データサイズ（2バイト） */
	DATA_PARSER_RECEIVING_BODY, 	/* データサイズ（2バイト） */
	DATA_PARSER_PROCESS_IO 			/* IO制御中 */	
} parserState_t ;

#define CMD_HEAD_SIZE 4	/**< \brief データサイズの文字数 */

#define COMMAND_NOP            0x0000  /**< \brief なにもしない（NOP） */

/* コマンド長は文字数ベース */
#define COMMAND_NOP_LEN                0x0008	/**< \brief COMMAND_NOP_LEN のコマンド長 */

/* parserの初期化処理 */
void initParser( hPipe_t id );

/* 受信リングバッファからデータを取得してコマンドをParseする
   送信データをリングバッファに格納したら、引数のハンドルに送信要求を発行する */
bool_t processParser ( hPipe_t id );

bool_t processCommand( hPipe_t id, uint8_t *buf, uint16_t size );

/* メモリチェック */
bool_t processCommand_MemoryCheck( hPipe_t id, uint8_t *buf );


/* ダンプリスト形式に編集された送信データを格納する */
void outDataDumpStyle( hPipe_t id, uint8_t *data, uint16_t size, uint16_t offset );

/* 送信コールバック関数 データ送信のシグナルを上げる */
void outDataRawStyle( hPipe_t id, uint8_t *data, uint16_t size );

/* 送信コールバック関数 データ送信のシグナルを上げる */
void outDataStringStyle( hPipe_t id, uint8_t *data );

void outputResult ( hPipe_t id, uint16_t *data );

#endif /* __PARSER_H__ */
