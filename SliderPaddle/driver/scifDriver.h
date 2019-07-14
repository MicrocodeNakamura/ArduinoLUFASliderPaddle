/*
 * u16Driver.h
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#ifndef __SCIFDRIVER_H__
#define __SCIFDRIVER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "commonDriver.h"
#include "ringBuffer.h"
#include "pipe.h"
/************************************************************************/
/* configure area                                                       */
/************************************************************************/
/* section UART ----- */
void init_UART ( hPipe_t pipe_id );
void flashUARTRingBuffer( hPipe_t id );
void registPipeUART ( hPipe_t id );
void uart_write ( uint8_t c );
#endif /* __U16DRIVER_H__ */
