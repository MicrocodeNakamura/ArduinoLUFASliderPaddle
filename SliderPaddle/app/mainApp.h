/*
 * mainApp.h
 *
 * Created: 2018/08/30
 * Author : ya_nakamura
 */ 
#ifndef __MAIN_APP_H__
#define __MAIN_APP_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"
#include "pipe.h"

#define USED_ADC_CH	3
#define ADC_CH_A	13
#define ADC_CH_B	14
#define ADC_CH_C	15

/* コントローラーの個数 */
#define PLAYERS	6

/* event flag */
#define RX_SIG 0x01	/**< UART 受信イベントシグナル */
#define TX_SIG 0x02	/**< UART 送信イベントシグナル（未使用） */

/* applicationの初期化処理 */
void initApp( void );

/* アプリケーション処理のメインループ */
void ApplicationTask ( bool_t fire );

/* 受信コールバック関数 データ受信のシグナルを上げる */
uint8_t rxSignal( hPipe_t prevId, hPipe_t id, uint16_t data);

/* 送信コールバック関数 データ送信のシグナルを上げる */
uint8_t txSignal( hPipe_t prevId, hPipe_t id, uint16_t data);

#endif /* __MAIN_APP_H__ */
