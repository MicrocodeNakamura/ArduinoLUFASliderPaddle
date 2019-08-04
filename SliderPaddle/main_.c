/*
 * adc8ch.c
 *
 * Created: 2018/09/24 22:55:33
 * Author : ya_nakamura
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
	
	/* Timer intialize */
	init_Timer( timer_handler );

	/* Enable Interrupt */
	enableInterrupt();

    /* Replace with your application code */
    while (1) 
    {
		bool_t fire;
		static bool_t led = FALSE;
		static uint8_t ledct = 0;

		fire = FALSE;
		if ( timerGetTime() > 15 ) {
			timerClearTime();
			fire = TRUE;		
			
			/* brink led.*/
			if ( ledct > 33 ) {
				led = ( led == TRUE ) ? FALSE : TRUE;
				setLED ( led );
				ledct = 0;
			} else {
				ledct++;
			}
		}

		ApplicationTask( fire );
    }
}


