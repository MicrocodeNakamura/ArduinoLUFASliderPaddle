/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the KeyboardMouse demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include <avr/io.h>
#include "typedef.h"
#include "commonDriver.h"
#include "gpioDriver.h"
#include "mainApp.h"
#include "timerDriver.h"
#include "adcDriver.h"

void setLED ( bool_t mode )
{
	if ( mode == TRUE ) {
		setMaskedGPIO(IOID_PORT_B, 0x80, 0x80 );
		} else {
		setMaskedGPIO(IOID_PORT_B, 0x00, 0x80 );
	}
}

int main(void)
{
	/* Disable Interrupt */
	disableInterrupt();
	
	/* Hardware initailze */
	initGPIO();

	/* Pipe initialize */
	initApp();

	/* ADC Initialize */
	initADC( 0xff00 );

	/* rotary controller initialize. */
	initRotary();
	
	/* Timer intialize */
	init_Timer( timer_handler );

	/* Enable Interrupt */
	enableInterrupt();

    /* Replace with your application code */
    while (1) 
    {
		bool_t fire;
		uint8_t mode = 0; // Uninitialize state
		uint8_t ledct_max = 16;
		static bool_t led = FALSE;
		static uint8_t ledct = 0;

#if 1
		if ( timerGetTime() > 30 ) {
			ledct++;
			if ( ledct > ledct_max ) {
				led = ( led == TRUE ) ? FALSE : TRUE;
				setLED ( led );
				ledct = 0;
			}
		}

		fire = FALSE;
		if ( timerGetTime() > 30 ) {
			timerClearTime();
			fire = TRUE;		
		}
		ApplicationTask( fire );

#else
		/* Before USB enumration. Main host is waiting start packet '0xaa' 2 times. */
		if ( mode == 0 ) {
		
			/* Start packet received successfully. Go to check gpio(PB6). */
			mode = 1;
			
		/* Check 'disable enumeration' gpio port. if enabled, go mode enumeration.  */
		else if ( mode == 1 ) {
			/* When B6 port(PWM12) low is USB enable.*/
			if ( ( getGPIO ( IOID_PORT_B ) & BIT6_VAL ) == 0 ) {
				mode = 2;
				ledct_max = 5;
			}
		}
		/* Send descriptor information. */
		else if ( mode == 2 ) {
			ledct_max = 33;
		}
		/* Start main routine. */
		else if ( mode == 3 ) {
			fire = FALSE;
			if ( timerGetTime() > 30 ) {
				timerClearTime();
				fire = TRUE;		
			}
			ApplicationTask( fire );
		}
#endif
    }
}
