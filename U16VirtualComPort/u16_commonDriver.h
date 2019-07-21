/*
 * u16Driver.h
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#ifndef __U16_COMMONDRIVER_H__
#define __U16_COMMONDRIVER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "u16_commonDriver.h"

typedef void (*InterruptCallbackHandler)(void);

void disableInterrupt( void );
void enableInterrupt( void );

#endif /* __U16DRIVER_H__ */
