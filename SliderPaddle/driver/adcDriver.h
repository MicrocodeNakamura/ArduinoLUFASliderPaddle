/*
 * adcDriver.h
 *
 * Created: 2018/09/24 23:48:34
 *  Author: ya_nakamura
 */ 


#ifndef ADCDRIVER_H_
#define ADCDRIVER_H_

/* ADC�`�����l���̍ő吔 */
#define MAX_ADC_CH 16

/* ADC�����҂��̃��W�X�^�ǂݏo���ő�񐔂̎w�� */
#define ADC_TIMEOUT 100

typedef enum {
	ADC_RESOLUTION_4bit,
	ADC_RESOLUTION_8bit,
	ADC_RESOLUTION_10bit,
	ADC_RESOLUTION_MAX
} adcMode_t;

/* adcCh ��1���Z�b�g���ꂽ�r�b�g�ɑΉ�����ADC�`���l���̏������B
   A4~A7�̓f�o�b�O�Ɏg�p����̂Ŏg�p�s�� */
void initADC( uint16_t adcCh );

/* ADC�̕���\��ݒ肷��B 4bit, 8bit, 10bit(RAW)�̂����ꂩ�B �ݒ�l��adcMode_t�͈̔͊O�Ȃ�΁AFalse��Ԃ� */
bool_t setADCResolution ( uint8_t ch, adcMode_t mode );

/* ADC�l���l������B ����ADC�̑��蒆�̏ꍇ�́AFalse��Ԃ� */
bool_t getADCValue ( uint8_t ch, uint16_t *data );

#endif /* ADCDRIVER_H_ */