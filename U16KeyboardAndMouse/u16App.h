/*
  u16App.h
*/
#ifndef __U16APP_H__
#define __U16APP_H__

#define MEGA2560_PACKETHEADER_SIZE 2
#define PAYLOAD_SIZE_LENGTH 4
#define PLAYERS 6
#define PAYLOAD_SIZE ( MEGA2560_PACKETHEADER_SIZE + PAYLOAD_SIZE_LENGTH + ( PLAYERS * 2 ) )

#define KEY_MAP_RIGHT_ALLOW 0
#define KEY_MAP_LEFT_ALLOW 1
#define KEY_MAP_BUTTON 2
#define KEY_MAP_MAX 3

typedef enum {
	PARSE_STATE_PREINIT,
	PARSE_STATE_INIT,
	PARSE_STATE_HEAD,
	PARSE_STATE_SIZE,
	PARSE_STATE_PAYLOAD,
	PARSE_STATE_MAX
} parseState_t;

typedef enum {
	ERROR_PARSE_DATA_LENGTH,
	ERROR_PARSE_MAX
} parseError_t;


typedef struct {
	uint8_t nopData[1];
}	cmdTypeNop_t;

typedef struct {
	uint8_t keyData[5];
}	cmdTypeKey_t;

/* �}�E�X�̐�Βl�A���Βl�͐擪��cmd�Ŕ��f����B
   �ݒ�ł���l�͈̔͂́AUSB_MouseReport_Data_t */
typedef struct {
	
}	cmdTypeMouse_t;


typedef union {
	cmdTypeNop_t nop;
	cmdTypeKey_t key;
	/* �}�E�X�̐�Βl�A���Βl�͐擪��cmd�Ŕ��f���� */
	cmdTypeMouse_t mouse;
} payload_ut;

typedef struct {
	uint8_t cmd;
	uint8_t size;
	payload_ut payload;
} cmdPacket_t;

/* �A�v���P�[�V�����^�X�N���C������ */
void appMain ( hPipe_t pipe_id );

/* UART��M�f�[�^�̃p�[�X���� */
uint8_t parseReceiveData ( hPipe_t pipe_id );

/* �X���C�h�{�����[����ADC�l���擾���� */
uint16_t get_slider_pos( void );

/* �p�h���̉�]�����Ɖ�]�p���擾���� */
int8_t *get_paddle_addr ( void );
int8_t *get_button_addr ( void );

/* IO�|�[�g�̃`�F�b�N���� */
// uint8_t pollIOPort ( void );

#endif /* __U16APP_H__ */

