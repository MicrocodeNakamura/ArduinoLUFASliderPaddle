/*
 * commonDriver.c
 *
 * Created: 2018/08/27
 * Author : ya_nakamura
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "commonDriver.h"
#include "typedef.h"

/* This macro will replace device common */

/**
*割り込み禁止 \n
**/
void disableInterrupt( void ){
	asm volatile( "cli" );
}

/**
* 割り込み許可関数 \n
**/
void enableInterrupt( void ){
	asm volatile( "sei" );
}

