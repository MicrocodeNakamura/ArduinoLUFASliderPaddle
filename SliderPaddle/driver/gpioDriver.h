/*
 * gpioDriver.h
 *
 * Created: 2018/8/27 1:35:32
 * Author : ya_nakamura
 */ 

#ifndef __GPIODRIVER_H__
#define __GPIODRIVER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "commonDriver.h"
#include "typedef.h"

typedef enum {
	 IOID_PORT_A,
	 IOID_PORT_B,
	 IOID_PORT_C,
	 IOID_PORT_D,
	 IOID_PORT_E,
	 IOID_PORT_F,
	 IOID_PORT_G,
	 IOID_PORT_H,
	 IOID_PORT_J,
	 IOID_PORT_K,
	 IOID_PORT_L,
	 IOID_PORT_MAX
} ioid_t;

#define BIT0_VAL 0x01
#define BIT1_VAL 0x02
#define BIT2_VAL 0x04
#define BIT3_VAL 0x08
#define BIT4_VAL 0x10
#define BIT5_VAL 0x20
#define BIT6_VAL 0x40
#define BIT7_VAL 0x80

/**
  gpioモードのセット。 GPIO_PREPAREとGPIO_STATE_MAX以上は選択不可。
  GPIO_STATE_PREPARE,	ポート未初期化
  GPIO_STATE_Z ,		バスバッファ＝Z、アドレス不定、RD
  GPIO_STATE_READ,	バスバッファ IN、アドレス保持、RD
  GPIO_STATE_WRITE,	バスバッファ OUT、アドレス保持、WR
  GPIO_STATE_MAX	無効

  Signal	IO										state	Z	read	write
  /CS	49:out 74LS256 ChipSelect(Low active)			PL0	H	L	L
  R/W	50:out 6502(34) Read(High)/Wrirte(Low)			PB3	H	H	L
  D-R/W	52:out 74LS256 Data direction ( H:A->B(WR) L:B<-A(RD) )	PB1	L	L	H
*/
typedef enum {
	GPIO_STATE_PREPARE,
	GPIO_STATE_Z ,
	GPIO_STATE_READ,
	GPIO_STATE_WRITE,
	GPIO_STATE_MAX
} gpioState_t;

void setGPIO ( ioid_t port, uint8_t out );
uint8_t getGPIO ( ioid_t port );
void setGPIOMODE ( ioid_t port, uint8_t out, uint8_t Z );
void setMaskedGPIO ( ioid_t port, uint8_t out, uint8_t mask );

/* GPIO API */

/* GPIOの初期化処理 */
void initGPIO (void);

/* gpioステートのセット。 GPIO_PREPAREとGPIO_STATE_MAX以上は選択不可 */
bool_t setGPIOState ( gpioState_t state );

/* アドレスバスに引数で指定された16ビットの値を設定する */
void setGPIOAddr ( uint16_t addr );

/* アドレスバスに引数で指定された16ビットの値を設定する */
void setGPIOData ( uint8_t data );

/* 
   IOを操作して、メモリからデータをリードする。
   IOの状態が未初期化のとき、またはアドレスdataがNULLならエラー（読み出し失敗）を返す */
bool_t readMemory ( uint16_t addr, uint8_t *data );

/* 
   IOを操作して、メモリにデータをライトする。
   IOの状態が未初期化のときはエラー（書き込み失敗）を返す */
bool_t writeMemory ( uint16_t addr, uint8_t data );

bool_t writeMemoryFill ( uint16_t addr, uint8_t data, uint16_t size );

uint8_t writeMemoryBlock ( uint16_t addr, uint8_t *data, uint16_t size );
#endif /* __U16DRIVER_H__ */
