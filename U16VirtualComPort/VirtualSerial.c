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
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "VirtualSerial.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           =
					{
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs.
 */
static FILE USBSerialStream;

#include "ringBuffer.h"
#include "u16_scifDriver.h"
#include "u16_commonDriver.h"
#include "pipe.h"

#define DEBUG_WORK_SIZE 64
uint8_t dbg[DEBUG_WORK_SIZE];

void init_work( void );

void init_work( void )
{
	uint16_t i;
	for ( i = 0 ; i < DEBUG_WORK_SIZE ; i++ ) {
		dbg[i] = 0;
	}
}

static bool_t scif_rx_flag = FALSE;
static bool_t scif_tx_flag = FALSE;

/* return 0 - 正常終了 */
static uint8_t rxCallback( hPipe_t id, hPipe_t previd, uint16_t data ) {
	/* 受信データのトリガをセットする */
	scif_rx_flag = TRUE;
	return 0;
}

static uint8_t txCallback( hPipe_t id, hPipe_t previd, uint16_t data ) {
	/* 送信データのトリガをセットする */
	scif_tx_flag = TRUE;
	return 0;
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
/* 38.4kbps TXD1(PWML1) -> com(RX), RXD(PWML0) -> com(TXD) */
int main(void)
{
	uint8_t c;
	uint16_t wk;
	hPipe_t pipe_id;
	
	init_work();
	SetupHardware();
	
	initMainPipe();
	/* PIPEの受信契機、送信契機検知コールバック関数が引数 */
	pipe_id = openDataPipe( rxCallback, txCallback );

	init_UART( pipe_id );
	registPipeUART ( pipe_id );

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();
	enableInterrupt();

	for (;;)
	{
//		CheckJoystickMovement();

		do {
			/* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
			/* USB データ受信処理 */
			wk = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
			if ( wk == 0xffff ) { break; }
			// uart_write ((uint8_t)(wk & 0x00ff));
			/* データ受信パイプ（USB→COM）にデータをセットしてトリガをかける */
			c = wk & 0x00ff;

			/* 送信元ID、送信先パイプID、データアドレス、転送バイト数 */
			setTxDataToPipe( 0, pipe_id, &c , 1 );
			/* 受信処理リクエスト発行 */
			reqTxPipe( 0, pipe_id, 1 );
		} while ( 1 );

		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();

		/* HOST PC(USB IN) <- (USB OUT)MEGA U16(UART RX) <- MEGA2560(UART TX) */
		if ( scif_rx_flag == TRUE ) {
			uint16_t size;
			uint8_t data[18];
			
			size = getRxPipeDataSize( pipe_id );
			/* 最大データサイズ16バイトでバースト送信 */
			if ( size > 16 ) { size = 16; }
			size = getRxDataFromPipe( pipe_id, data, size);
			data[size] = '\0';
			/* USB送信処理呼び出し */
			fputs((const char *)data, &USBSerialStream);
			scif_rx_flag = FALSE;
		}
		
		/* HOST PC(USB OUT) -> (USB IN)MEGA U16(UART TX) -> MEGA2560(UART RX) */
		if ( scif_tx_flag == TRUE ) {
			uart_write( pipe_id );
			scif_tx_flag = FALSE;
		}
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	USB_Init();
}

/** Checks for changes in the position of the board joystick, sending strings to the host upon each change. */
void CheckJoystickMovement(void)
{
	// not used function
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** CDC class driver callback function the processing of changes to the virtual
 *  control lines sent from the host..
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
	/* You can get changes to the virtual CDC lines in this callback; a common
	   use-case is to use the Data Terminal Ready (DTR) flag to enable and
	   disable CDC communications in your application when set to avoid the
	   application blocking while waiting for a host to become ready and read
	   in the pending data from the USB endpoints.
	*/
	bool HostReady = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
}
