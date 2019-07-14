/*
 * scifDriver.c
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "scifDriver.h"
#include "ringBuffer.h"
#include "pipe.h"

// static void uart_write ( unsigned char c );
static void rx_handler( void );

static InterruptCallbackHandler uartRXint = NULL;
static InterruptCallbackHandler uartTXint = NULL;

/* バッファオーバーフローしたデータの量がカウントアップされる */
static hPipe_t scif_pipe_id = PIPE_INVALID;
static uint16_t readBufferOverflow = 0;

/* Interrupt vector registration. ----- */

/**
\brief UART機能の受信ハードウェア割り込みハンドラ。

USART1でデータを１バイト受信するたびにハードウェアから呼び出される関数\n
指定された受信パイプのリングバッファに受信データを格納し、受信処理\n
リクエストを発行する

\retval	なし
**/
ISR(USART0_RX_vect){
	/* uartRXint is function pointer. */
	if ( uartRXint != NULL ){ uartRXint(); }
}

ISR(USART0_UDRE_vect){
	/* uartTXint is function pointer */
	if ( uartTXint != NULL ){ uartTXint(); }
}
/**
  \brief UART機能の受信ハンドラ。 受信ハードウェア割り込み契機で呼び出される\n

  指定された受信パイプのリングバッファに受信データを格納し、受信処理\n
  リクエストを発行する\n

\retval	なし
**/
/* interrupt handler ----- */
static void rx_handler( void ){
	unsigned char c;

	c = UDR0;         // read rx receive buffer
	/* data write to ring buffer. */

	/* PIPEの未初期化ステート時はデータを捨てる処理を追加 TBD */

	/* 送信元ID、送信先パイプID、データアドレス、転送バイト数 */
	if ( setRxDataFromPipe( 0, scif_pipe_id, &c , 1 ) == 0 ){
		readBufferOverflow++;
	} else {
		/* 受信処理リクエスト発行 */
		reqRxPipe( 0, scif_pipe_id, 1 );		
	}
}

/* interrupt handler ----- */
static void tx_handler( void ){
	uint8_t data;
	if ( getTxDataFromPipe( scif_pipe_id, &data , 1 ) == 1 ){			
		UDR0 = data;
	} else {
		UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0);
	}
}

//-------------------------------------------
// Section UART
//-------------------------------------------
/**
\brief UART機能の初期化。 固定ボーレートを設定し、UART機能を有効にする

pipe idを指定することで、送受信バッファと送受信契機を指定する\n

\param	pipe_id	有効なpipe idを指定する
\retval	なし
**/
void init_UART ( hPipe_t pipe_id ){
	// set Baudrate to 115.2kbps
	UBRR0H = 0;
//	UBRR0L = 103; // 9600 bps
//	UBRR0L = 25;  // 38400 bps
	UBRR0L = 12;  // 76800 bps 12MHz = 57600bps
//	UBRR0L = 7;   // 115200 bps (not supported)

	UCSR0C = (1<<USBS0)|(3<<UCSZ10);
	/* 送信及び受信と送信受信割り込みを有効化する */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0);
	
	uartRXint = rx_handler;
	uartTXint = tx_handler;
	
	/* PIPE IDを保存 */
	scif_pipe_id = pipe_id;
}

/**
\brief UARTドライバにpipeを登録する

送受信契機は登録したpipe idのコールバック関数を通じて通知される\n
データ送信時にはUART側の送信契機関数が呼び出され、\n
データ受信時には上位側の受信契機関数を呼び出す。\n

\param id 登録するpipe ID
\retval	なし
**/
void registPipeUART ( hPipe_t id ) {
	scif_pipe_id = id;
}

/**
\brief UARTへデータ送信をレジスタレベルで行う

UART送信レジスタに１バイトのデータを出力する

\param	c	出力するデータ
\retval	なし
**/
void uart_write ( hPipe_t id ){
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(0<<TXCIE0)|(1<<UDRIE0);
}

/**
\brief 送信リングバッファに残っている全てのデータを出力する

全てのデータ送信が終わるまで、関数を抜けない\n
1バイト送信するごとに送信完了待ちウェイトが発生する\n

\param	id	バッファフラッシュを行うpipe ID
\retval	なし
**/
#if 0
void flashUARTRingBuffer( hPipe_t id ) {
	uint8_t data[18];
	uint16_t size, i;
	size = getTxPipeDataSize( id );
	while ( size != 0 ){
		if ( size > 16 ) { size = 16; }
		size = getTxDataFromPipe( id, data, size);
		for ( i = 0 ; i < size ; i++ ) {
				uart_write( data[i] );
			}
		}
		size = getTxPipeDataSize( id );
	}
}
#endif
