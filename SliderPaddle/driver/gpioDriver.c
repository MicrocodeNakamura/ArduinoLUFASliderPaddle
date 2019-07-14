/*
 * gpioDriver.c
 *
 * Created: 2018/08/27
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "commonDriver.h"
#include "gpioDriver.h"
uint8_t portmask[IOID_PORT_MAX];
uint8_t portpull[IOID_PORT_MAX];

/* GPIO バスステート保存変数 */
static uint8_t flipBit ( uint8_t in );

/**
\brief GPIOの初期設定

GPIOの入出力設定と、GPIOステートの初期化を行う

\retval	なし
**/
void initGPIO ( void ) {
	setGPIOMODE ( IOID_PORT_B, 0x8A, 0x00 );
}

/**
\brief マスク付きGPIO出力

出力前の状態との論理和で出力ポートの特定ビット状態のみを変化させる

\param	port	出力先ポートid
\param	out	出力データ
\param	mask	マスクビット、1がセットされたビットのみ変更対象
\retval	なし
**/
void setMaskedGPIO ( ioid_t port, uint8_t out, uint8_t mask ) {
	uint8_t buf;
	switch ( port ){
		case IOID_PORT_A: buf = PORTA; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_B: buf = PORTB; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_C: buf = PORTC; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_D: buf = PORTD; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_E: buf = PORTE; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_F: buf = PORTF; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_G: buf = PORTG; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_H: buf = PORTH; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_J: buf = PORTJ; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_K: buf = PORTK; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		case IOID_PORT_L: buf = PORTL; buf = (( ~mask ) & buf) | ( mask & out ); setGPIO(port,buf); break;
		default:
			break;
	}
}

/**
\brief GPIOに値をセットする

入力ポートに対応するビットを更新すると、プルアップ設定が変更されるので\n
注意すること

\param	port	IOのid名で出力先ポートを指定する
\param	out	出力値
\retval	なし
**/
void setGPIO ( ioid_t port, uint8_t out )
{
	switch ( port ) {
		case IOID_PORT_A: PORTA = ( out & portmask[IOID_PORT_A] ) | portpull[IOID_PORT_A]; break;
		case IOID_PORT_B: PORTB = ( out & portmask[IOID_PORT_B] ) | portpull[IOID_PORT_B]; break;
		case IOID_PORT_C: PORTC = ( out & portmask[IOID_PORT_C] ) | portpull[IOID_PORT_C]; break;
		case IOID_PORT_D: PORTD = ( out & portmask[IOID_PORT_D] ) | portpull[IOID_PORT_D]; break;
		case IOID_PORT_E: PORTE = ( out & portmask[IOID_PORT_E] ) | portpull[IOID_PORT_E]; break;
		case IOID_PORT_F: PORTF = ( out & portmask[IOID_PORT_F] ) | portpull[IOID_PORT_F]; break;
		case IOID_PORT_G: PORTG = ( out & portmask[IOID_PORT_G] ) | portpull[IOID_PORT_G]; break;
		case IOID_PORT_H: PORTH = ( out & portmask[IOID_PORT_H] ) | portpull[IOID_PORT_H]; break;
		case IOID_PORT_J: PORTJ = ( out & portmask[IOID_PORT_J] ) | portpull[IOID_PORT_J]; break;
		case IOID_PORT_K: PORTK = ( out & portmask[IOID_PORT_K] ) | portpull[IOID_PORT_K]; break;
		case IOID_PORT_L: PORTL = ( out & portmask[IOID_PORT_L] ) | portpull[IOID_PORT_L]; break;
		/* error */
		default:
		break;				
	}
}

/**
\brief GPIOから値を取得する

\param	port	値を取得するポートをIOのidで指定
\retval	取得した値
**/
uint8_t getGPIO ( uint8_t port )
{
	uint8_t ret = 0;	
	switch ( port ) {
		case IOID_PORT_A: ret = PINA; break;
		case IOID_PORT_B: ret = PINB; break;
		case IOID_PORT_C: ret = PINC; break;
		case IOID_PORT_D: ret = PIND; break;
		case IOID_PORT_E: ret = PINE; break;
		case IOID_PORT_F: ret = PINF; break;
		case IOID_PORT_G: ret = PING; break;
		case IOID_PORT_H: ret = PINH; break;
		case IOID_PORT_J: ret = PINJ; break;
		case IOID_PORT_K: ret = PINK; break;
		case IOID_PORT_L: ret = PINL; break;

		/* error */
		default:
		break;
	}
	
	return ret;
}

/* out input mode = 0 (default), output mode = 1
   none pull-up register = 0, add pull-up register = 1
*/
/**
\brief GPIOの入出力モードを指定する

\param	port	モード設定を行うポートをidで指定する
\param	out	入出力を指定する。 0なら入力、1なら出力
\param	Z	ハイインピーダンス指定
\retval	なし
**/
void setGPIOMODE ( ioid_t port, uint8_t out, uint8_t Z )
{
	switch ( port ) {
		case IOID_PORT_A: DDRA = out; PORTA = (PORTA&(out))|Z; portmask[IOID_PORT_A] = out; portpull[IOID_PORT_A] = Z; break;
		case IOID_PORT_B: DDRB = out; PORTB = (PORTB&(out))|Z; portmask[IOID_PORT_B] = out; portpull[IOID_PORT_B] = Z; break;
		case IOID_PORT_C: DDRC = out; PORTC = (PORTC&(out))|Z; portmask[IOID_PORT_C] = out; portpull[IOID_PORT_C] = Z; break;
		case IOID_PORT_D: DDRD = out; PORTD = (PORTD&(out))|Z; portmask[IOID_PORT_D] = out; portpull[IOID_PORT_D] = Z; break;
		case IOID_PORT_E: DDRE = out; PORTE = (PORTE&(out))|Z; portmask[IOID_PORT_E] = out; portpull[IOID_PORT_E] = Z; break;
		case IOID_PORT_F: DDRF = out; PORTF = (PORTF&(out))|Z; portmask[IOID_PORT_F] = out; portpull[IOID_PORT_F] = Z; break;
		case IOID_PORT_G: DDRG = out; PORTG = (PORTG&(out))|Z; portmask[IOID_PORT_G] = out; portpull[IOID_PORT_G] = Z; break;
		case IOID_PORT_H: DDRH = out; PORTH = (PORTH&(out))|Z; portmask[IOID_PORT_H] = out; portpull[IOID_PORT_H] = Z; break;
		case IOID_PORT_J: DDRJ = out; PORTJ = (PORTJ&(out))|Z; portmask[IOID_PORT_J] = out; portpull[IOID_PORT_J] = Z; break;
		case IOID_PORT_K: DDRK = out; PORTK = (PORTK&(out))|Z; portmask[IOID_PORT_K] = out; portpull[IOID_PORT_K] = Z; break;
		case IOID_PORT_L: DDRL = out; PORTL = (PORTL&(out))|Z; portmask[IOID_PORT_L] = out; portpull[IOID_PORT_L] = Z; break;
		/* error */
		default:
		break;
	}
}

/**
\brief ビットの順逆を入れ替える 7bit -> 0bitのように。

\param	in	ビット反転させる対象の値
\retval	演算結果
**/
static uint8_t flipBit ( uint8_t in ) {
	uint8_t i,s,c;
	s = in;
	for ( i=0, c=0; i < 8 ; i++ ) {
		c = c << 1;
		c = c + ( (s>>i) & 0x01);  
	}
	return c;
}

