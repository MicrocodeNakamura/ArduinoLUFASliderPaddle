/*
 u16App.c
 KeyboardMouse.c のタスクループから呼び出される処理。
*/

#include "typedef.h"
#include "pipe.h"
#include "u16App.h"

/* 取得したadc値の保存用変数 */
static uint16_t adc_pos = 0;
static int8_t paddle[PLAYERS * 2];
static int8_t button[PLAYERS];

static uint8_t size_err = 0;
static uint8_t head_err = 0;

uint16_t get_slider_pos( void )
{
	return adc_pos;	
}

int8_t *get_paddle_addr ( void )
{
	/* 受信したpaddleデータのバッファを格納 */
	return paddle;	
}

int8_t *get_button_addr ( void )
{
	/* 受信したpaddleデータのバッファを格納 */
	return button;
}

/* UART受信データのパース処理 - clitical section */
/*
  AA AA
  [size(0x0000001E) 4]
  [Player1 keycode 2] [Player2 keycode 2] [Player3 keycode 2]
  [Player1 keycode 2] [Player1 keycode 2] [Player1 keycode 2]
  player keycoddeは、PADの回転角数、ボタンの押下（１）の組み合わせ。　PADの回転なし、押下なしの場合は0x00、0x00
*/

uint8_t parseReceiveData ( hPipe_t pipe_id ) {
	static parseState_t parseState = PARSE_STATE_PREINIT;
	static uint8_t rest = 0;
	static uint8_t size[4];
	static uint8_t payload[ PAYLOAD_SIZE ];
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
			/* サイズ値は PAYLOAD_SIZE 固定 */
			if ( (size[0] == 0x00) && (size[1] == 0x00) && (size[2] == 0x00) && (size[3] == PAYLOAD_SIZE) ){
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
		uint8_t i;
		rest -= getRxDataFromPipe( pipe_id, &payload[PAYLOAD_SIZE - rest], rest);
		if ( rest == 0 ) {
			/* State を元に戻す */
			parseState = PARSE_STATE_HEAD;
			rest = 2;
			
			/* build data */
			for ( i = 0 ; i < PLAYERS ; i++ ) {
				paddle[i] += (int8_t)(payload[i*2]);
				if ( payload[i*2 + 1] != 0 ) {
					button[i] = 1; /* ボタン押下を検知した */
				}
			}
		}
	}
	
	return 0;
}


/* application main 処理 */
void appMain ( hPipe_t pipe_id ) {
	// pollIOPort();
	
	parseReceiveData( pipe_id );
}
