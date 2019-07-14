#include "stdio.h"
#include "commDriver.h"
#include "typedef.h"

#include "ringBuffer.h"
// pipe controller
static pipeMainControll_t pipeMain;
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

/* �f�[�^���MAPI�֐� */
uint8_t stdTxFunc ( hPipe_t id, hPipe_t senderid, uint8_t *data, uint16_t size ) {
	if ( pipeMain.pipe[id].stdTxFunc != NULL ) {
		/* Send data to driver. */
		return pipeMain.pipe[id].stdRxFunc( id, senderid, data, size );
	}
}

/* pipeAPI�Ŏ�M�����f�[�^�̕W���������[�`���B �|�C���^�֐���ύX���邱�Ƃ�
   �I�[�o�[���C�h�\ */
uint8_t stdRxFunc( hPipe_t id, hPipe_t senderid, uint8_t *data, uint16_t size) {
	/* �w�肳�ꂽpipeid�̃����O�o�b�t�@�Ƀf�[�^���i�[���� */
	if ( id >= PIPE_MAX ) {
		return /* INVALID_ID */ -1;
	}
	if ( pipeMain.isUse[id] != TRUE ) {
		return /* NOTUSED */ -1;
	}
	
	/* Send data to application. */
	return pipeMain.pipe[id].stdTxFunc( id, senderid, data, size );
}


/*
  Pipe�\���̂ƃo�b�t�@���Ђ��t����ID���擾����B
  API��IP���g���ăA�N�Z�X����B 
  �擾����PIPE ID���h���C�oAPI�̏����������ɓn�����ƂŁA�n�[�h�E�F�A�`���l����_���`���l���ɕϊ�����
*/
hPipe_t openDataPipe( uint8_t *buf, cbFunc_t stdRxFunc, cbFunc_t stdTxFunc ) {
	uint8_t id;
	uint8_t ret = 0xff;
	for ( id = 0 ; id < PIPE_MAX ; id++ ) {
		if ( pipeMain.isUse[id] == FALSE ) {
			break;
		}
	}

	if ( id != PIPE_MAX ) {
		pipeMain.isUse[id] = TRUE;
		pipeMain.pipe[id].pBuf = buf;
		pipeMain.pipe[id].pAppData = NULL;
		pipeMain.pipe[id].streamType = 0xffff;        // TBD
		pipeMain.pipe[id].streamID = 0xff;  // TBD
		/*  */
		pipeMain.pipe[id].stdRxFunc = stdRxFunc;  /* �W����M�R�[���o�b�N�֐� */
		pipeMain.pipe[id].stdTxFunc = stdTxFunc;  /* �W�����M�R�[���o�b�N�֐� */

		initRingBuffer( &(pipeMain.pipe[id].rx_ring), pipeBuffer[id], PIPE_BUFFER_SIZE );
		initRingBuffer( &(pipeMain.pipe[id].tx_ring), pipeBuffer[id], PIPE_BUFFER_SIZE );
		ret = id;
	}
	return id;
}

/* �w�肳�ꂽPIPE ID�̃f�[�^�o�b�t�@�擪�̈���擾���� */

#if 0
/* pipe�ŗL�̃f�[�^�o�b�t�@�A�h���X�B�@���荞�݃n���h�����o�b�t�@���Q�Ƃ���悤�ȃP�[�X�Ŏg�p���� */
uint8_t *getPipeDataBuffer ( hPipe_t id ) {
	uint8_t *ret = NULL;
	if ( id < PIPE_MAX ) {
		if ( pipeMain.isUse[id] == TRUE ) {
			ret = pipeMain.pipe[id].pBuf;
		}
	}

	return ret;
}
#endif

#if 0
/* �w�肳�ꂽPIPE ID�̃����O�o�b�t�@�Ǘ����̃A�h���X��Ԃ� */

/* ��M�h���C�o���荞�ݏ�������M�����f�[�^�������O�o�b�t�@�Ɋi�[����悤�ȃP�[�X�Ŏg�p���� */
ringBuffer_t *getRingBufferAddr ( hPipe_t id ) {
	ringBuffer_t *ret = NULL;
	if ( id < PIPE_MAX ) {
		if ( pipeMain.isUse[id] == TRUE ) {
			ret = &(pipeMain.pipe[id].ring);
		}
	}

	return ret;
} 
#endif

/* ���M�A�v���P�[�V���������Ăяo��API */
uint16_t setTxDataToPipe ( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	pipeMain.pipe[id].stdTxFunc( prev_id, id, buf, size );
	return ret;
}

/* ���M�A�v���P�[�V���������Ăяo��API */
uint16_t setRxDataFromPipe ( hPipe_t prev_id, hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	pipeMain.pipe[id].stdRxFunc( prev_id, id, buf, size );
	return ret;
}

uint16_t getTxDataFromPipe ( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].tx_ring), buf, size );
	return ret;
}

uint16_t getRxDataFromPipe ( hPipe_t id, uint8_t *buf, uint16_t size ) {
	uint16_t ret;
	ret = getRingBuffers( &(pipeMain.pipe[id].rx_ring), buf, size );
	return ret;
}

/* ��M�A�v���P�[�V���������Ăяo��API �T�C�Y�擾 */
uint16_t getTxDataSizeToPipe ( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].tx_ring) );
	return ret;
}

/* ��M�A�v���P�[�V���������Ăяo��API �T�C�Y�擾 */
uint16_t getRxDataSizeFromPipe ( hPipe_t id ) {
	uint16_t ret;
	ret = getRingBufferDataSize( &(pipeMain.pipe[id].rx_ring) );
	return ret;
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
