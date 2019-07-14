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
#include "parser.h"

static uint8_t readBuffer[1024];

static parserState_t pState = DATA_PARSER_NO_INIT;
static uint8_t receiveBuf[32];

static bool_t chg4Hex2Dec ( uint8_t *ch, uint16_t *data );
static bool_t chg2Hex2Dec ( uint8_t *ch, uint8_t *data );
static bool_t memCheck( uint16_t addr, uint8_t data, uint16_t size, uint16_t *err_addr );

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
\brief メモリチェック。 指定された範囲のメモリのリードライトチェック

指定されたアドレス範囲のメモリに指定されたデータを書き込み、同じデータが読み出せるかを
確認する。

\param	*ch	変換する文字列の格納アドレス
\retval	*data	変換後の数値
\note なし
**/
static bool_t memCheck ( uint16_t addr, uint8_t data, uint16_t size, uint16_t *err_addr )
{
	bool_t ret = TRUE;
	uint16_t i;
	uint8_t check;
	
	for ( i = 0 ; i < size ; i++ ) {
		(void)writeMemory ( addr, data );
		(void)readMemory  ( addr, &check );
		if ( data != check ) {
			ret = FALSE;
			/* RAMエラーが出た番地を呼び出し元に返す */
			if ( err_addr != NULL ) {
				*err_addr = addr;
			}
			break;
		}
	}
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
	pState = DATA_PARSER_RECEIVING_SIZE;
	/* 受信データバッファの内容物をクリア */

/* タイトルは無し */
#if 0
	while ( getRxDataFromPipe( id, receiveBuf, sizeof(receiveBuf) ) != 0 );
	outDataStringStyle( id, (uint8_t *)"Parser initialized.\n" );
	outDataStringStyle( id, (uint8_t *)"6502 mini ICE ver 1.00. Programed by microcode.\n" );
#endif
}

/**
\brief パーサのメイン処理

受信リングバッファからデータを取得してコマンドをParseする\n
送信データをリングバッファに格納したら、引数のハンドルに送信要求を発行し\n
接続先ホストPCに応答を返す

\param	id	送受信を行うpipe id
\retval	正常終了ならTRUE、異常終了ならFALSEを返す
\note なし
**/
bool_t processParser ( hPipe_t id )
{
	static uint16_t datasize = 0;
	uint16_t rxsize = 0;
	bool_t ret = FALSE;
	bool_t loop = TRUE;
	
	/* データサイズまたはデータボディ受信中のみParser処理が動作する。 */
	while ( loop == TRUE ) {
		switch ( pState ) {
			case DATA_PARSER_RECEIVING_SIZE:
				rxsize = getRxPipeDataSize( id );
				/* 先頭4文字でデータサイズを獲得。獲得に失敗した場合、ステートはそのままで、
				   次の先頭4文字を探す。 受信データバッファの残量が3以下になればループを終了。 */
				if ( rxsize >= CMD_HEAD_SIZE ) {
				/* 先頭4文字がデータサイズ。
					サイズチェック済みなのでリングバッファのデータ取得チェックはしない */
					(void)getRxDataFromPipe( id, receiveBuf, CMD_HEAD_SIZE );
					/* 先頭4文字のHEXを数値化する */
					if ( chg4Hex2Dec( receiveBuf, &datasize ) == TRUE )
					{
						/* コマンド本体受信ステートへ遷移する */
						pState = DATA_PARSER_RECEIVING_BODY;
					}
				} else {
					/* バッファが3文字以下なので、ループ終了 */
					loop = FALSE;
				}
				break;
			
			case DATA_PARSER_RECEIVING_BODY:
				rxsize = getRxPipeDataSize( id );
				/* 受信バッファを先に4バイト消費しているので、要求受信データサイズを4バイトを少なくする */
				if ( rxsize >= (datasize-4) ) {
					/* バッファにデータサイズ部分を残して、残りコマンドデータを受信する。
					   機能間IFではないので、IFのデータ定義が甘かった。 反省。
					   サイズチェック済みなのでリングバッファのデータ取得チェックはしない */
					(void)getRxDataFromPipe( id, &receiveBuf[4], datasize );
					ret = processCommand ( id, receiveBuf, datasize );
					
					if ( ret == TRUE) {
						outDataStringStyle( id, (uint8_t *)"ok\n" );
					}
					else
					{
						/* error code output */
						outDataStringStyle( id, (uint8_t *)"ng\n" );
					}
						
					/* コマンドデータサイズ受信状態に遷移する */
					pState = DATA_PARSER_RECEIVING_SIZE;
					/* １コマンド終了したので、処理をメインループへ返す */
					loop = FALSE;
				}
				else
				{
					/* バッファが規定文字以下なので、parserの状態を維持したままループ終了 */
					loop = FALSE;
				}
				break;
			default:
				/* ここには来ない */
				/* 不正なステート中に受信したデータは全て廃棄する。 */
				while ( getRxDataFromPipe( id, receiveBuf, sizeof(receiveBuf) ) != 0 );
				pState = DATA_PARSER_RECEIVING_SIZE;
				loop = FALSE;
				break;
		}
	}

	return ret;
}

/**
\brief 受信したデータの内容を一次解析して、適切な処理関数を呼び出す

一連の処理を実行して、正常終了ならOKの文字\n
異常終了だった場合はエラーコードとメッセージ（オプション）をUARTで\n
接続先ホストPCに送信する

\param	id	応答データを送信するために使うpipe id
\param	*buf	受信データが格納されているバッファ
\param	size	受信データサイズ
\retval	正常終了時はTRUEを返す
\note 異常終了時にこの関数内部からエラー詳細を返すが、\n
呼び元でエラー応答を再度返しても構わない
**/
bool_t processCommand( hPipe_t id, uint8_t *buf, uint16_t size )
{
	uint16_t cmd;
	bool_t ret = FALSE;
	
	if ( chg4Hex2Dec( &receiveBuf[4], &cmd ) == TRUE ) {
		switch ( cmd ) {
			case COMMAND_NOP:
				/* なにもしない（テストコード記述） */
				if ( size == COMMAND_NOP_LEN ) {
					ret = TRUE;
				}
				break;

			default:
				break;
		}
	}
	
	return ret;
}

/**
\brief メモリチェック

0010 0006 xxxx yyyy\n
メモリ自動チェック。 アドレスxxxxからyyyyバイト数\n
00->55->AA->FF->00の順番で書き込みとベリファイを行う。\n
ベリファイでエラーが発生した場合、発生アドレスと書き込み失敗データを\n
レスポンスに返す。 成功したら、StatusOKを返す。

\param	id	応答を送信するためのpipe id
\param	buf	入力データ格納アドレス
\retval	正常終了ならTRUEを返す
\note なし
**/
bool_t processCommand_MemoryCheck( hPipe_t id, uint8_t *buf )
{
	uint16_t addr, size;
	uint16_t err_addr;
	uint8_t check;
	bool_t ret = TRUE;

	if ( chg4Hex2Dec( (uint8_t *)(&buf[8]), &addr ) != TRUE ) { ret = FALSE; }
	if ( chg4Hex2Dec( (uint8_t *)(&buf[12]), &size ) != TRUE ) { ret = FALSE; }

	if ( ret == FALSE ) { return FALSE; }

	if ( ret == TRUE ) { check = (uint8_t)0x00; ret = memCheck( addr, check, size, &err_addr ); }
	if ( ret == TRUE ) { check = (uint8_t)0x55; ret = memCheck( addr, check, size, &err_addr ); }
	if ( ret == TRUE ) { check = (uint8_t)0xAA; ret = memCheck( addr, check, size, &err_addr ); }
	if ( ret == TRUE ) { check = (uint8_t)0xFF; ret = memCheck( addr, check, size, &err_addr ); }
	if ( ret == TRUE ) { check = (uint8_t)0x00; ret = memCheck( addr, check, size, &err_addr ); }

	/* retの値がFALSEだったとき、書き込み先RAMアドレスと書き込んだデータをHOSTPCに返す(TBD) */
	return ret;
}

/**
\brief ダンプリスト形式に編集された送信データをHOSTPCへ送信する

\param	*data	送信対象データ格納アドレス
\param	size	送信データサイズ
\retval	なし
\note なし
**/
static uint8_t dump[128];

void outDataDumpStyle( hPipe_t id, uint8_t *data, uint16_t size, uint16_t offset ){
	uint8_t i;
	uint16_t last,len;
	uint16_t hex;
	
	last = size;
	while ( last != 0 ) {
		if ( last >= 16 ) {
			len = 16;
			last -= 16;
		} else {
			len = last;
			last = 0;
		}
		
		/* オフセットアドレスの出力 */
		hex = chgDec2Hex ( (offset >> 8) & 0x00ff );
		dump[0] = ( hex >> 8 ) & 0x00ff;
		dump[1] = hex & 0x00ff;
		hex = chgDec2Hex ( offset & 0x00ff );
		dump[2] = ( hex >> 8 ) & 0x00ff;
		dump[3] = hex & 0x00ff;
		dump[4] = ':';

		/* １行の出力 */
		for ( i = 0 ; i < len ; i++ ) {
			hex = chgDec2Hex ( *data );
			dump[5 + (i*3) + 0] = ( hex >> 8 ) & 0x00FF;
			dump[5 + (i*3) + 1] = hex & 0x00FF;
			dump[5 + (i*3) + 2] = ' ';
			data++;
		}
		offset += 0x0010;

		/* 改行コード */
		dump[5 + (i*3) + 0] = '\n';
		
		/* ダンプデータをUART送信バッファに格納 データ長は改行分まで*/
		setTxDataToPipe ( (hPipe_t)0, id, dump , ( len * 3 ) + 6 );
		reqTxPipe ( (hPipe_t)0, id, ( len * 3 ) + 3);
	}
}

/**
\brief 生(RAW)データ形式でHOSTPCにデータ送信する

左端にアドレスを表示する

\param	*data	送信対象データ格納アドレス
\param	size	送信データサイズ
\param	offset	アドレス表示用のオフセット値
\retval	なし
\note なし
**/
void outDataRawStyle( hPipe_t id, uint8_t *data, uint16_t size ){
	uint16_t last,len,i;
	
	last = size;
	while ( last != 0 ) {
		if ( last >= 16 ) {
			len = 16;
			last -= 16;
		} else {
			len = last;
			last = 0;
		}

		for ( i = 0 ; i < len ; i++ ) {
			dump[i] = *data;
			data++;
		}

		/* ダンプデータをUART送信バッファに格納 データ長は改行分まで*/
		setTxDataToPipe ( (hPipe_t)0, id, dump , len );
		reqTxPipe ( (hPipe_t)0, id, len );
	}
}

/**
\brief String形式のデータをHOSTPCに送信する

メッセージ送信などに使用する汎用的な文字列送信API関数。
ASCII文字のみ送信可能。

\param	*data	送信対象データ格納アドレス
\retval	なし
\note なし
**/
void outDataStringStyle( hPipe_t id, uint8_t *data ){
	uint8_t strLen = 0;
	
	while ( ( *data != '\0' ) && ( strLen < sizeof (dump) ) ) {
		dump[strLen] = *data;
		data++;
		strLen++;
	}

	/* 文字列データをUART送信バッファに格納し、送信要求を発行する */
	setTxDataToPipe ( (hPipe_t)0, id, dump , strLen );
	reqTxPipe ( (hPipe_t)0, id, strLen );
}

static uint8_t outDatas[ 8 + (4*8) ];
void outputResult ( hPipe_t id, uint16_t *data ){
	uint16_t tmp;
	uint8_t i;
	outDatas[0] = 'A';
	outDatas[1] = 'A';
	outDatas[2] = 'A';
	outDatas[3] = 'A';

	tmp = chgDec2Hex ( (uint8_t)( sizeof(outDatas) & 0x00ff ) );
	outDatas[4] = (uint8_t)('0');
	outDatas[5] = (uint8_t)('0');
	outDatas[6] = (uint8_t)(( tmp >> 8 ) & 0x00ff);
	outDatas[7] = (uint8_t)( tmp & 0x00ff );

	for ( i = 0 ; i < 8 ; i++ ) {
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

