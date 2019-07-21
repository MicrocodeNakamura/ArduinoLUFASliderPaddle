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

/* �o�b�t�@�I�[�o�[�t���[�����f�[�^�̗ʂ��J�E���g�A�b�v����� */
static hPipe_t scif_pipe_id = PIPE_INVALID;
static uint16_t readBufferOverflow = 0;

/* Interrupt vector registration. ----- */

/**
  \brief UART�@�\�̎�M�n���h���B ��M�n�[�h�E�F�A���荞�݌_�@�ŌĂяo�����\n

  �w�肳�ꂽ��M�p�C�v�̃����O�o�b�t�@�Ɏ�M�f�[�^���i�[���A��M����\n
  ���N�G�X�g�𔭍s����\n

\retval	�Ȃ�
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
  \brief UART�@�\�̎�M�n���h���B ��M�n�[�h�E�F�A���荞�݌_�@�ŌĂяo�����\n

  �w�肳�ꂽ��M�p�C�v�̃����O�o�b�t�@�Ɏ�M�f�[�^���i�[���A��M����\n
  ���N�G�X�g�𔭍s����\n

\retval	�Ȃ�
**/
/* interrupt handler ----- */
static void rx_handler( void ){
	unsigned char c;

	c = UDR1;         // read rx receive buffer
	/* data write to ring buffer. */

	// debug. local loop character.
	// UDR1 = c;

	/* ���M��ID�A���M��p�C�vID�A�f�[�^�A�h���X�A�]���o�C�g�� */
	if ( setRxDataFromPipe( 0, scif_pipe_id, &c , 1 ) == 0 ){			
		readBufferOverflow++;
	} else {
		/* ��M�������N�G�X�g���s */
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
\brief UART�@�\�̏������B �Œ�{�[���[�g��ݒ肵�AUART�@�\��L���ɂ���

pipe id���w�肷�邱�ƂŁA����M�o�b�t�@�Ƒ���M�_�@���w�肷��\n

\param	pipe_id	�L����pipe id���w�肷��
\retval	�Ȃ�
**/
void init_UART ( hPipe_t pipe_id ){
	PIND = 0xff;
	DDRD = 0x80;

	// set Baudrate to 115.2kbps
	UBRR1H = 0;
//	UBRR1L = 102; // 9600 bps ok
//	UBRR1L = 25;  // 38400 bps ok
	UBRR1L = 12;  // 76800 bps
	/* 115.2Kbps�̓N���b�N�̌������K�v�����AUSB��D�悷�邽�߃N���b�N�����͕s�� */
//	UBRR1L = 7;   // 115200 bps NG

	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
	/* ���M�y�ю�M�Ƒ��M��M���荞�݂�L�������� */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1)|(0<<UDRIE1);
	
	uartRXint = rx_handler;
	uartTXint = tx_handler;
	/* PIPE ID��ۑ� */
	scif_pipe_id = pipe_id;

	DDRB = 0xE0;
}

/* UART��Pipe��o�^����B
  �f�[�^���M���ɂ�UART���̑��M�_�@�֐����Ăяo����A
  �f�[�^��M���ɂ͏�ʑ��̎�M�_�@�֐����Ăяo���B
  */
void registPipeUART ( hPipe_t id ) {
	scif_pipe_id = id;
}

/* �����֐��B UART�̑��M�����W�X�^���x���ōs�� */
void uart_write ( hPipe_t id ){
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(0<<TXCIE1)|(1<<UDRIE1);
}

/* Ring buffer �Ɋi�[���ꂽ���M�f�[�^��S��UART�ő��M���� */
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
