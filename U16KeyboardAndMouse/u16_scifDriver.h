/*
 * u16Driver.h
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#ifndef __U16_SCIFDRIVER_H__
#define __U16_SCIFDRIVER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16_commonDriver.h"
#include "ringBuffer.h"
#include "pipe.h"
/************************************************************************/
/* configure area                                                                      */
/************************************************************************/
/* section UART ----- */
void init_UART ( hPipe_t pipe_id );
void registPipeUART ( hPipe_t id );

// debug
void uart_write ( unsigned char c );

#endif /* __U16DRIVER_H__ */
