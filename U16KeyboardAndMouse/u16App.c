/*
 u16App.c
 KeyboardMouse.c のタスクループから呼び出される処理。
*/

#include "typedef.h"
#include "pipe.h"
#include "u16App.h"

#define MEGA2560_PACKETHEADER_SIZE 2

/* 取得したadc値の保存用変数 */
static uint16_t adc_pos = 0;
static int8_t paddle = 0;

static uint8_t size_err = 0;
static uint8_t head_err = 0;

uint16_t get_slider_pos( void )
{
	return adc_pos;	
}

int8_t get_paddle_val ( void )
{
	int8_t dummy = paddle;
	/* paddle情報は検知割り込みごとに積算され、読み出すたびにリセットされる。 */
	paddle = 0;
	return dummy;	
}

/* UART受信データのパース処理 */
uint8_t parseReceiveData ( hPipe_t pipe_id ) {
	static parseState_t parseState = PARSE_STATE_INIT;
	static uint8_t rest = 0;
	static uint8_t size[4];
	static uint8_t payload[7];
	uint8_t tmp;
	
	/* パーサーのイニシャライズ */
	if ( parseState == PARSE_STATE_PREINIT ) {
		/* ドライバ等のイニシャライズ */
		parseState = PARSE_STATE_INIT;
	} else if ( parseState == PARSE_STATE_INIT ) {
		parseState = PARSE_STATE_HEAD;
		/* 次のステートで受信が必要なバイト数（固定値 2） */
		rest = MEGA2560_PACKETHEADER_SIZE;
	/* パケット先頭１バイトとサイズ1バイトを取得 */
	} else if ( parseState == PARSE_STATE_HEAD ) {
		/* UARTの受信パケットを１バイト単位で読み出し */
		while ( rest != 0 ) {
			/* 受信データがなければ、ループ完了。 あれば未受信データ数を-1して再度読み出し */
			if ( getRxDataFromPipe( pipe_id, &tmp, 1) == 1 ) {
				if ( tmp != 0xaa ) {
					/* ヘッダ2バイト読むまでパケットと認めない */
					rest = MEGA2560_PACKETHEADER_SIZE;
					head_err++;
				} else {
					rest--;
				}
				if ( rest == 0 ) {
					/* 2バイト検知したので、残りのデータ読み出しステートへ */
					rest = 0x04;
					parseState = PARSE_STATE_SIZE;
					break;
				}
			} else {
				break;
			}
		}
	} else if ( parseState == PARSE_STATE_SIZE ) {
		/* 4 バイトサイズデータ待ち */
		rest -= getRxDataFromPipe( pipe_id, &size[sizeof(size) - rest], rest);
		if ( rest == 0 ) {
			/* サイズ値は0x0d固定 */
			if ( (size[0] == 0x00) && (size[1] == 0x00) && (size[2] == 0x00) && (size[3] == 0x0d) ){
				parseState = PARSE_STATE_PAYLOAD;
				rest = 7;
			} else {
				parseState = PARSE_STATE_HEAD;
				rest = 2;
				size_err++;
			}
		}
	/* PAYLOAD部分の受信 */
	} else if ( parseState == PARSE_STATE_PAYLOAD ) {
		rest -= getRxDataFromPipe( pipe_id, &payload[sizeof(payload) - rest], rest);
		if ( rest == 0 ) {
			/* State を元に戻す */
			parseState = PARSE_STATE_HEAD;
			rest = 2;
			
			/* build data */
			paddle += payload[6];
			adc_pos = ( payload[2] << 8 ) + payload[3];
		}
	}
	
	return 0;
}


/* application main 処理 */
void appMain ( hPipe_t pipe_id ) {
	// pollIOPort();
	
	parseReceiveData( pipe_id );
}
