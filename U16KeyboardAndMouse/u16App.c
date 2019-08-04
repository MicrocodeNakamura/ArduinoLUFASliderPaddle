/*
 u16App.c
 KeyboardMouse.c �̃^�X�N���[�v����Ăяo����鏈���B
*/

#include "typedef.h"
#include "pipe.h"
#include "u16App.h"

/* �擾����adc�l�̕ۑ��p�ϐ� */
static uint16_t adc_pos = 0;
static int8_t paddle[PLAYERS * 2];
static int8_t button[PLAYERS];

static uint8_t size_err = 0;
static uint8_t head_err = 0;

uint16_t get_slider_pos( void )
{
	return adc_pos;	
}

int8_t *get_paddle_addr ( void )
{
	/* ��M����paddle�f�[�^�̃o�b�t�@���i�[ */
	return paddle;	
}

int8_t *get_button_addr ( void )
{
	/* ��M����paddle�f�[�^�̃o�b�t�@���i�[ */
	return button;
}

/* UART��M�f�[�^�̃p�[�X���� - clitical section */
/*
  AA AA
  [size(0x0000001E) 4]
  [Player1 keycode 2] [Player2 keycode 2] [Player3 keycode 2]
  [Player1 keycode 2] [Player1 keycode 2] [Player1 keycode 2]
  player keycodde�́APAD�̉�]�p���A�{�^���̉����i�P�j�̑g�ݍ��킹�B�@PAD�̉�]�Ȃ��A�����Ȃ��̏ꍇ��0x00�A0x00
*/

uint8_t parseReceiveData ( hPipe_t pipe_id ) {
	static parseState_t parseState = PARSE_STATE_PREINIT;
	static uint8_t rest = 0;
	static uint8_t size[4];
	static uint8_t payload[ PAYLOAD_SIZE ];
	uint8_t tmp;
	
	/* �p�[�T�[�̃C�j�V�����C�Y */
	if ( parseState == PARSE_STATE_PREINIT ) {
		/* �h���C�o���̃C�j�V�����C�Y */
		parseState = PARSE_STATE_INIT;
	} else if ( parseState == PARSE_STATE_INIT ) {
		parseState = PARSE_STATE_HEAD;
		/* ���̃X�e�[�g�Ŏ�M���K�v�ȃo�C�g���i�Œ�l 2�j */
		rest = MEGA2560_PACKETHEADER_SIZE;
	/* �p�P�b�g�擪�P�o�C�g�ƃT�C�Y1�o�C�g���擾 */
	} else if ( parseState == PARSE_STATE_HEAD ) {
		/* UART�̎�M�p�P�b�g���P�o�C�g�P�ʂœǂݏo�� */
		while ( rest != 0 ) {
			/* ��M�f�[�^���Ȃ���΁A���[�v�����B ����Ζ���M�f�[�^����-1���čēx�ǂݏo�� */
			if ( getRxDataFromPipe( pipe_id, &tmp, 1) == 1 ) {
				if ( tmp != 0xaa ) {
					/* �w�b�_2�o�C�g�ǂނ܂Ńp�P�b�g�ƔF�߂Ȃ� */
					rest = MEGA2560_PACKETHEADER_SIZE;
					head_err++;
				} else {
					rest--;
				}
				if ( rest == 0 ) {
					/* 2�o�C�g���m�����̂ŁA�c��̃f�[�^�ǂݏo���X�e�[�g�� */
					rest = 0x04;
					parseState = PARSE_STATE_SIZE;
					break;
				}
			} else {
				break;
			}
		}
	} else if ( parseState == PARSE_STATE_SIZE ) {
		/* 4 �o�C�g�T�C�Y�f�[�^�҂� */
		rest -= getRxDataFromPipe( pipe_id, &size[sizeof(size) - rest], rest);
		if ( rest == 0 ) {
			/* �T�C�Y�l�� PAYLOAD_SIZE �Œ� */
			if ( (size[0] == 0x00) && (size[1] == 0x00) && (size[2] == 0x00) && (size[3] == PAYLOAD_SIZE) ){
				parseState = PARSE_STATE_PAYLOAD;
				rest = 7;
			} else {
				parseState = PARSE_STATE_HEAD;
				rest = 2;
				size_err++;
			}
		}
	/* PAYLOAD�����̎�M */
	} else if ( parseState == PARSE_STATE_PAYLOAD ) {
		uint8_t i;
		rest -= getRxDataFromPipe( pipe_id, &payload[PAYLOAD_SIZE - rest], rest);
		if ( rest == 0 ) {
			/* State �����ɖ߂� */
			parseState = PARSE_STATE_HEAD;
			rest = 2;
			
			/* build data */
			for ( i = 0 ; i < PLAYERS ; i++ ) {
				paddle[i] += (int8_t)(payload[i*2]);
				if ( payload[i*2 + 1] != 0 ) {
					button[i] = 1; /* �{�^�����������m���� */
				}
			}
		}
	}
	
	return 0;
}


/* application main ���� */
void appMain ( hPipe_t pipe_id ) {
	// pollIOPort();
	
	parseReceiveData( pipe_id );
}
