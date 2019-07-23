/**
\brief �ʐM�C���^�[�t�F�[�X�ڑ��T�[�r�X
*/

#include "stdio.h"
#include "u16_commonDriver.h"
#include "typedef.h"

#include "ringBuffer.h"
#include "pipe.h"

// pipe controller
/* static */pipeMainControll_t pipeMain;
static uint8_t pipeBuffer[PIPE_MAX][PIPE_BUFFER_SIZE];

void testcb( hPipe_t previd, hPipe_t id, uint8_t *buf, uint16_t size ) {
	/* dummy */
}

/*
recv ��M�o�b�t�@�Ǘ��\����
*/
void initMainPipe ( void ) {
	uint8_t i;
	for ( i = 0 ; i < PIPE_MAX ; i++ ){
		pipeMain.isUse[i] = FALSE;
	}
}

/*
  Pipe�\���̂ƃo�b�t�@���Ђ��t����ID���擾����B
  API��IP���g���ăA�N�Z�X����B 
  �擾����PIPE ID���h���C�oAPI�̏����������ɓn�����ƂŁA�n�[�h�E�F�A�`���l����_���`���l���ɕϊ�����
*/
hPipe_t openDataPipe( cbFunc_t stdRxFunc, cbFunc_t stdTxFunc ) {
	uint8_t id;

	for ( id = 0 ; id < PIPE_MAX ; id++ ) {
		if ( pipeMain.isUse[id] == FALSE ) {
			break;
		}
	}

	if ( id != PIPE_MAX ) {
		pipeMain.isUse[id] = TRUE;
		pipeMain.pipe[id].streamType = 0xffff;        // TBD
		pipeMain.pipe[id].streamID = 0xff;  // TBD
		/*  */
		pipeMain.pipe[id].stdRxFunc = stdRxFunc;  /* �W����M�R�[���o�b�N�֐� */
		pipeMain.pipe[id].stdTxFunc = stdTxFunc;  /* �W�����M�R�[���o�b�N�֐� */

		initRingBuffer( &(pipeMain.pipe[id].rx_ring), pipeBuffer[id], PIPE_BUFFER_SIZE );
		initRingBuffer( &(pipeMain.pipe[id].tx_ring), pipeBuffer[id+1], PIPE_BUFFER_SIZE );
	} else {
		id = PIPE_ERROR_ID;
	}
	return id;
}

/* �f�[�^����M�v���́A�f�[�^�̏����g���K�̔��s�֐����Ăяo���B
   ���Ƃ��΁A��M�ł���΃n���h�����炱�̊֐����Ăяo�����ƂŁABottomHalf�����_�@����邱�Ƃ��ł��� */
/* �f�[�^��M�v���iApp <- Driver) */
uint8_t reqRxPipe ( hPipe_t senderid, hPipe_t id, uint16_t size ) {
	if ( ( id >= PIPE_MAX ) || ( pipeMain.isUse[id] != TRUE ) ){
		return PIPE_ERROR_ID;
	}

	if ( pipeMain.pipe[id].stdRxFunc != NULL ) {
		/* Send data to driver. */
		return pipeMain.pipe[id].stdRxFunc( senderid, id, size );
	}
	/* pipe handle is not found. */
	return PIPE_ERROR_ID;
}

/* �f�[�^���M�v���iApp -> Driver) */
uint8_t reqTxPipe( hPipe_t senderid, hPipe_t id, uint16_t size) {
	if ( ( id >= PIPE_MAX ) || ( pipeMain.isUse[id] != TRUE ) ){
		return /* INVALID_ID */ -1;
	}
	/* Send data to application. */
	return pipeMain.pipe[id].stdTxFunc( senderid, id, size );
}

/* ���M�f�[�^�iApp -> Driver) �������O�o�b�t�@�ɃZ�b�g */
uint16_t setTxDataToPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = setRingBuffers ( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

/* ��M�f�[�^�iApp <- Driver) �������O�o�b�t�@�ɃZ�b�g */
uint16_t setRxDataFromPipe( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = setRingBuffers ( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/* ���M�f�[�^�iApp -> Driver) �������O�o�b�t�@����擾 */
uint16_t getTxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

/* ��M�f�[�^�iApp <- Driver) �������O�o�b�t�@����擾 */
uint16_t getRxDataFromPipe( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/* ��M�A�v���P�[�V���������Ăяo��API �T�C�Y�擾 */
uint16_t getTxPipeDataSize( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].tx_ring) );
	return ret;
}

/* ��M�A�v���P�[�V���������Ăяo��API �T�C�Y�擾 */
uint16_t getRxPipeDataSize( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].rx_ring) );
	return ret;
}

bool_t isRxPipeIsEmpty( hPipe_t id ) {
	if ( isRingBufferEmpty( &(pipeMain.pipe[id].rx_ring) ) ){
		return TRUE;
	} else {
		return FALSE;		
	}
}

bool_t isTxPipeIsEmpty( hPipe_t id ) {
	if ( isRingBufferEmpty( &(pipeMain.pipe[id].tx_ring) ) ){
		return TRUE;
		} else {
		return FALSE;
	}
}

#if 0
/* callback routine template */
uint8_t rxStdCallback( hPipe_t, hPipe_t, uint8_t *, uint16_t);
uint8_t txStdCallback( hPipe_t, hPipe_t, uint8_t *, uint16_t);

uint8_t rxStdCallback( hPipe_t prev_id, hPipe_t id, uint8_t *buffer, uint16_t size)
{
	return 0;
}

uint8_t txStdCallback( hPipe_t prev_id, hPipe_t id, uint8_t *buffer, uint16_t size)
{
	return 0;
}
#endif