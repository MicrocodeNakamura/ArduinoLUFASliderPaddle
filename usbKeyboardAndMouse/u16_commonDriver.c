/*
 * u16Driver.c
 *
 * Created: 2018/08/05
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "u16_commonDriver.h"
#include "u16_timerDriver.h"

//-------------------------------------------
// Section UART
//-------------------------------------------
/* This macro will replace device common */
void disableInterrupt( void ){
	asm volatile( "cli" );
}

void enableInterrupt( void ){
	asm volatile( "sei" );
}
