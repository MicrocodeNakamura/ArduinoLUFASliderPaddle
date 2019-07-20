/*
 * adcDriver.c
 *
 * Created: 2018/09/24 23:48:17
 *  Author: ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "typedef.h"

#include "adcDriver.h"
#include "gpioDriver.h"

/* ADC 分解能の初期値は10bit */
static adcMode_t resolution[MAX_ADC_CH];

/* adcCh に1がセットされたビットに対応するADCチャネルの初期化。
   A4~A7はデバッグに使用するので使用不可 */
void initADC( uint16_t ch )
{
	uint8_t i;
	for ( i = 0 ; i < MAX_ADC_CH ; i++ ) {
		resolution[i] = ADC_RESOLUTION_10bit;
	}
	
	ADMUX = (0<<REFS1)|(1<<REFS0);	/* AVCCを基準電源に設定 */
	ADCSRA = (1<<ADEN)|(0<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);	/* ADCの電源ON、A/D変換クロック選択(Clk/4) */
	DIDR0 = ch & 0x00ff;
	DIDR2 = (uint8_t)((( ch & 0xff00 ) >> 8) & 0xff);
}

/* ADCの分解能を設定する。 4bit, 8bit, 10bit(RAW)のいずれか。 設定値がadcMode_tの範囲外ならば、Falseを返す */
bool_t setADCResolution ( uint8_t ch, adcMode_t mode )
{
	bool_t ret = FALSE;
	if ( mode < ADC_RESOLUTION_MAX ) {
		if ( ch < MAX_ADC_CH ) {
			resolution[ch] = mode;
			ret = TRUE;
		}
	}
	return ret;
}

/* ADC値を獲得する。 現在ADCの測定中の場合は、Falseを返す */
bool_t getADCValue ( uint8_t ch, uint16_t *data )
{
	bool_t ret = FALSE;
	uint16_t outdata;
	uint16_t i;

	/* チャネルが範囲内かをチェックする */
	if ( ch < MAX_ADC_CH ) {
		/* ADC動作中であれば、エラーを返す */
		if ( (ADCSRA & (1<<ADSC)) == 0 )
		{
			/* MUX5を1にセット */
			if ( ch > 7 ) {
				ADCSRB = (1<<MUX5);
			} else {
				ADCSRB = (0<<MUX5);
			}

			/* ADC読み出しチャネルセット AREF(REFS:00)を選択 */
			ADMUX = (0<<REFS1)|(0<<REFS0)|(ch & 0x07);

			/* ADSCを1にセットして、ADC変換開始 (空読み) */
			ADCSRA = (1<<ADEN) | (1<<ADPS1)|(1<<ADSC);				
			while ( (ADCSRA & (1<<ADSC)) != 0 );

			/* ADSCを1にセットして、ADC変換開始 */
			ADCSRA = (1<<ADEN) | (1<<ADPS1)|(1<<ADSC);				
		
			/* タイムアウト時間までに変換が完了していれば正常終了を返す */
			for ( i = 0 ; i < ADC_TIMEOUT ; i++ ){
				/* ADC変換が完了したかのチェック */
				if ( (ADCSRA & (1<<ADSC)) == 0 ) {
					outdata = ( ADCW & 0x3FF );
					/* 分解能にあわせて値を整形する */
					if ( resolution[ch] == ADC_RESOLUTION_4bit ) {
						outdata = (outdata >> 6) & 0x0f;
					} else if ( resolution[ch] == ADC_RESOLUTION_8bit ) {
						outdata = (outdata >> 2) & 0x0f;
					}
					*data = outdata;
					ret = TRUE;
					break;
				}
			}
		} else {
			ret = FALSE;
		}
	}
	return ret;
}

