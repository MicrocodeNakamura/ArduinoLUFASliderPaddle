#include "stdio.h"
#include "typedef.h"
#include "ringBuffer.h"

/**
\brief リングバッファの初期化

リングバッファが使用するパラメータを初期化する

\param	*ring	リングバッファ管理構造体
\param	buffer	データバッファの先頭 
\param	size	データバッファのサイズ
\retval	なし
\note なし
**/
void initRingBuffer( ringBuffer_t *ring, uint8_t *buffer, uint16_t size ){
	ring->buf = buffer;
	ring->rp = 0;
	ring->wp = 0;
	ring->size = size;
}

/**
\brief リングバッファにデータをセットする

\param	*ring	リングバッファ管理構造体
\param	data	セットするデータ
\retval	正常終了ならTRUE、異常終了ならFALSEを返す
\note なし
**/
bool_t setRingBuffer( ringBuffer_t *ring, uint8_t data )
{
	bool_t ret = FALSE;
	
	// disable interrupt
	if ( ((ring->wp+1) % ring->size) != ring->rp ){
		ring->buf[ring->wp] = data;
		ring->wp = (ring->wp + 1) % ring->size;
		ret = TRUE;
	}
	// enable interrupt
	return ret;
}

/**
\brief リングバッファにデータをゲットする

\param	*ring	リングバッファ管理構造体
\param	data	ゲットしたデータを格納するアドレス
\retval	正常終了ならTRUE、異常終了ならFALSEを返す
\note なし
**/
bool_t getRingBuffer( ringBuffer_t *ring, uint8_t *data )
{
	bool_t ret = FALSE;
	
	// disable interrupt
	if ( (ring->rp != ring->wp ) ){
		*data = ring->buf[ring->rp];
		ring->rp = (ring->rp + 1) % ring->size;
		ret = TRUE;
	}
	// enable interrupt

	return ret;
}

/**
\brief リングバッファに複数のデータをセットする

\param	*ring	リングバッファ管理構造体
\param	*data	セットするデータの先頭アドレス
\param	size	セットするデータサイズ
\retval	セットできたデータサイズを返す
\note なし
**/
uint16_t setRingBuffers( ringBuffer_t *ring, uint8_t *data, uint16_t size )
{
	uint16_t i;
	uint8_t *pt;
	pt = data;
	
	for ( i = 0 ; i < size ; i++ ) {
		if ( setRingBuffer( ring, *pt ) == 0 ) {
			break;
		}
		pt++;
	}
	return i;
}

/**
\brief リングバッファから複数のデータをゲットする

\param	*ring	リングバッファ管理構造体
\param	*data	ゲットしたデータを格納する先頭アドレス
\param	size	ゲットするデータサイズ
\retval	ゲットできたデータサイズを返す
\note なし
**/
uint16_t getRingBuffers( ringBuffer_t *ring, uint8_t *data, uint16_t size )
{
	uint16_t i;
	uint8_t *pt;
	pt = data;
	
	for ( i = 0 ; i < size ; i++ ) {
		if ( getRingBuffer( ring, pt ) == 0 ) {
			break;
		}
		pt++;
	}
	return i;
}

/**
\brief リングバッファのデータサイズを取得する

\param	*ring	リングバッファ管理構造体
\retval	リングサイズに格納されているデータのサイズ
\note なし
**/
uint16_t getRingBufferDataSize( ringBuffer_t *ring ) {
	uint16_t size;
	if ( ring->wp < ring->rp ) {
		size = ( ring->wp + ring->size ) - ring->rp;
	} else {
		size = ring->wp - ring->rp;
	}
	return size;
}

/**
\brief リングバッファが空かそうでないかを判別する

\param	*ring	リングバッファ管理構造体
\retval	データが格納されていなければTRUE、データが格納されていればFALSEを返す
\note なし
**/
bool_t isRingBufferEmpty( ringBuffer_t *ring )
{
	bool_t ret = FALSE;
	if ( ring->wp == ring->rp ) {
		ret = TRUE;
	}
	return ret;
}

/**
\brief リングバッファが満杯かそうでないかを判別する

\param	*ring	リングバッファ管理構造体
\retval	リングバッファが満杯であればTRUE、空き領域があればFALSEを返す
\note なし
**/
bool_t isRingBufferFull( ringBuffer_t *ring )
{
	bool_t ret = FALSE;
	if ( ((ring->wp+1) % ring->size) == ring->rp ) {
		ret = TRUE;
	}
	return ret;
}

#ifdef FEATURE_TESTCODE
#include "ringBuffer.f"
#endif /* FEATURE_TESTCODE */

