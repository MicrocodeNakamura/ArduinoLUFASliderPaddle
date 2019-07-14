/*
 * adcDriver.h
 *
 * Created: 2018/09/24 23:48:34
 *  Author: ya_nakamura
 */ 


#ifndef ADCDRIVER_H_
#define ADCDRIVER_H_

/* ADCチャンネルの最大数 */
#define MAX_ADC_CH 16

/* ADC完了待ちのレジスタ読み出し最大回数の指定 */
#define ADC_TIMEOUT 100

typedef enum {
	ADC_RESOLUTION_4bit,
	ADC_RESOLUTION_8bit,
	ADC_RESOLUTION_10bit,
	ADC_RESOLUTION_MAX
} adcMode_t;

/* adcCh に1がセットされたビットに対応するADCチャネルの初期化。
   A4~A7はデバッグに使用するので使用不可 */
void initADC( uint16_t adcCh );

/* ADCの分解能を設定する。 4bit, 8bit, 10bit(RAW)のいずれか。 設定値がadcMode_tの範囲外ならば、Falseを返す */
bool_t setADCResolution ( uint8_t ch, adcMode_t mode );

/* ADC値を獲得する。 現在ADCの測定中の場合は、Falseを返す */
bool_t getADCValue ( uint8_t ch, uint16_t *data );

#endif /* ADCDRIVER_H_ */