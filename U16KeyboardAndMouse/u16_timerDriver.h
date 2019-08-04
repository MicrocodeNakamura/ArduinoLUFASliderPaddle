/*
 * u16_timerDriver.h
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#ifndef __U16_TIMERDRIVER_H__
#define __U16_TIMERDRIVER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16_commonDriver.h"
/************************************************************************/
/* configure area                                                                      */
/************************************************************************/
/* section timer ----- */
/* base system clock 16.00MHz - pre-scaler */
#define TIMER_CLKDIV_0    (unsigned char)(0x00)
#define TIMER_CLKDIV_1    (unsigned char)(0x01)
#define TIMER_CLKDIV_8    (unsigned char)(0x02)
#define TIMER_CLKDIV_64   (unsigned char)(0x03)
#define TIMER_CLKDIV_256  (unsigned char)(0x04)
#define TIMER_CLKDIV_1024 (unsigned char)(0x05)

void init_Timer (
InterruptCallbackHandler handler ,
/* you can specify TIMER_CLKDIV_[n] in deviceint.h */
unsigned char TimerUnitMode ,
unsigned char time );


#endif /* __U16DRIVER_H__ */
