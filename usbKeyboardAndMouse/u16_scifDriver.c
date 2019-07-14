/*
 * u16Driver.c
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16_scifDriver.h"
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
  \brief UART機能の受信ハンドラ。 受信ハードウェア割り込み契機で呼び出される\n

  指定された受信パイプのリングバッファに受信データを格納し、受信処理\n
  リクエストを発行する\n

\retval	なし
**/
ISR(USART1_RX_vect){
	/* uartRXint is function pointer. */
	if ( uartRXint != NULL ){ uartRXint(); }
}

ISR(USART1_UDRE_vect){
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

	c = UDR1;         // read rx receive buffer
	/* data write to ring buffer. */

	// debug. local loop character.
	// UDR1 = c;

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
	if ( isTxPipeIsEmpty( scif_pipe_id ) != TRUE )
	{
		getTxDataFromPipe( scif_pipe_id, &data , 1 );			
		UDR1 = data;
	} else {
		UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1)|(0<<UDRIE1);
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
	PIND = 0xff;
	DDRD = 0x80;

	// set Baudrate to 115.2kbps
	UBRR1H = 0;
//	UBRR1L = 102; // 9600 bps ok
//	UBRR1L = 25;  // 38400 bps ok
	UBRR1L = 12;  // 76800 bps
	/* 115.2Kbpsはクロックの交換が必要だが、USBを優先するためクロック交換は不可 */
//	UBRR1L = 7;   // 115200 bps NG

	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
	/* 送信及び受信と送信受信割り込みを有効化する */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1)|(0<<UDRIE1);
	
	uartRXint = rx_handler;
	uartTXint = tx_handler;
	/* PIPE IDを保存 */
	scif_pipe_id = pipe_id;

	DDRB = 0xE0;
}

/* UARTにPipeを登録する。
  データ送信時にはUART側の送信契機関数が呼び出され、
  データ受信時には上位側の受信契機関数を呼び出す。
  */
void registPipeUART ( hPipe_t id ) {
	scif_pipe_id = id;
}

/* 内部関数。 UARTの送信をレジスタレベルで行う */
void uart_write ( hPipe_t id ){
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1)|(1<<UDRIE1);
}

/* Ring buffer に格納された送信データを全てUARTで送信する */
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
		size = getTxPipeDataSize( id );
	}
}
#endif
