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

/* ADC ����\�̏����l��10bit */
static adcMode_t resolution[MAX_ADC_CH];

/* adcCh ��1���Z�b�g���ꂽ�r�b�g�ɑΉ�����ADC�`���l���̏������B
   A4~A7�̓f�o�b�O�Ɏg�p����̂Ŏg�p�s�� */
void initADC( uint16_t ch )
{
	uint8_t i;
	for ( i = 0 ; i < MAX_ADC_CH ; i++ ) {
		resolution[i] = ADC_RESOLUTION_10bit;
	}
	
	ADMUX = (0<<REFS1)|(1<<REFS0);	/* AVCC����d���ɐݒ� */
	ADCSRA = (1<<ADEN)|(0<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);	/* ADC�̓d��ON�AA/D�ϊ��N���b�N�I��(Clk/4) */
	DIDR0 = ch & 0x00ff;
	DIDR2 = (uint8_t)((( ch & 0xff00 ) >> 8) & 0xff);
}

/* ADC�̕���\��ݒ肷��B 4bit, 8bit, 10bit(RAW)�̂����ꂩ�B �ݒ�l��adcMode_t�͈̔͊O�Ȃ�΁AFalse��Ԃ� */
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

/* ADC�l���l������B ����ADC�̑��蒆�̏ꍇ�́AFalse��Ԃ� */
bool_t getADCValue ( uint8_t ch, uint16_t *data )
{
	bool_t ret = FALSE;
	uint16_t outdata;
	uint16_t i;

	/* �`���l�����͈͓������`�F�b�N���� */
	if ( ch < MAX_ADC_CH ) {
		/* ADC���쒆�ł���΁A�G���[��Ԃ� */
		if ( (ADCSRA & (1<<ADSC)) == 0 )
		{
			/* MUX5��1�ɃZ�b�g */
			if ( ch > 7 ) {
				ADCSRB = (1<<MUX5);
			} else {
				ADCSRB = (0<<MUX5);
			}

			/* ADC�ǂݏo���`���l���Z�b�g AREF(REFS:00)��I�� */
			ADMUX = (0<<REFS1)|(0<<REFS0)|(ch & 0x07);

			/* ADSC��1�ɃZ�b�g���āAADC�ϊ��J�n (��ǂ�) */
			ADCSRA = (1<<ADEN) | (1<<ADPS1)|(1<<ADSC);				
			while ( (ADCSRA & (1<<ADSC)) != 0 );

			/* ADSC��1�ɃZ�b�g���āAADC�ϊ��J�n */
			ADCSRA = (1<<ADEN) | (1<<ADPS1)|(1<<ADSC);				
		
			/* �^�C���A�E�g���Ԃ܂łɕϊ����������Ă���ΐ���I����Ԃ� */
			for ( i = 0 ; i < ADC_TIMEOUT ; i++ ){
				/* ADC�ϊ��������������̃`�F�b�N */
				if ( (ADCSRA & (1<<ADSC)) == 0 ) {
					outdata = ( ADCW & 0x3FF );
					/* ����\�ɂ��킹�Ēl�𐮌`���� */
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

