/*
 * u16Driver.h
 *
 * Created: 2018/02/06 1:35:32
 * Author : ya_nakamura
 */ 

#ifndef __U16DRIVER_H__
#define __U16DRIVER_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

typedef unsigned char bool_t;
/************************************************************************/
/* configure area                                                                      */
/************************************************************************/
#define PORT_SCAN_TIMING_MS 10
#undef DEBUG_CHECK_USB_REPORT
#undef MAIN_TASK_KILL_SWITCH_ENABLE

/* Heartbeat led port setting */
#define DEBUG_LED_PORT PORTB
#define DEBUG_LED_BIT PORTB6

/* debug */
extern uint8_t debugFlag;

/* Interrupt ----- */
typedef void (*InterruptCallbackHandler)(void);

/* section GPIO ----- */
#define PB4_OUT_HIGH() PORTB |= (1<<4)
#define PB5_OUT_HIGH() PORTB |= (1<<5)
#define PB6_OUT_HIGH() PORTB |= (1<<6)
#define PB7_OUT_HIGH() PORTB |= (1<<7)

#define PB4_OUT_LOW() PORTB &= (~(1<<4))
#define PB5_OUT_LOW() PORTB &= (~(1<<5))
#define PB6_OUT_LOW() PORTB &= (~(1<<6))
#define PB7_OUT_LOW() PORTB &= (~(1<<7))

#define PB4_IN() (((PINB&(1<<4))!=0)?1:0)
#define PB5_IN() (((PINB&(1<<5))!=0)?1:0)
#define PB6_IN() (((PINB&(1<<6))!=0)?1:0)
#define PB7_IN() (((PINB&(1<<7))!=0)?1:0)

void init_GPIO ( void );
bool_t isUSBDeviceEnable( void );

/* section ADC ----- */
void init_ADC (void);
unsigned char getADC(unsigned char mux );

void init_u16Driver(void);

/* main task */
extern uint8_t breaker;

#endif /* __U16DRIVER_H__ */
