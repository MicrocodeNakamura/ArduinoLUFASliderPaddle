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

#include "KeyboardMouse.h"
#include "u16Driver.h"
#include "u16_scifDriver.h"
#include "pipe.h"
#include "u16App.h"

#include "../src/LUFA/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h"
#include "u16_timerDriver.h"

const uint8_t keycodemap[ PLAYERS ][ KEY_MAP_MAX ] = {
	{ HID_KEYBOARD_SC_RIGHT_ARROW, HID_KEYBOARD_SC_LEFT_ARROW, HID_KEYBOARD_SC_SPACE,  },
	{ HID_KEYBOARD_SC_4_AND_DOLLAR, HID_KEYBOARD_SC_5_AND_PERCENTAGE, HID_KEYBOARD_SC_6_AND_CARET,  },
	{ HID_KEYBOARD_SC_7_AND_AMPERSAND, HID_KEYBOARD_SC_8_AND_ASTERISK, HID_KEYBOARD_SC_9_AND_OPENING_PARENTHESIS,  },
	{ HID_KEYBOARD_SC_Q, HID_KEYBOARD_SC_W, HID_KEYBOARD_SC_E,  },
	{ HID_KEYBOARD_SC_R, HID_KEYBOARD_SC_T, HID_KEYBOARD_SC_Y,  },
	{ HID_KEYBOARD_SC_U, HID_KEYBOARD_SC_I, HID_KEYBOARD_SC_O,  }
};
	
/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** Buffer to hold the previously generated Mouse HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_MouseReport_Data_t)];

extern uint8_t KeyEnterStatus;

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the keyboard HID
 *  interface within the device.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_Keyboard,
				.ReportINEndpoint             =
					{
						.Address              = KEYBOARD_IN_EPADDR,
						.Size                 = HID_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
			},
	};

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the mouse HID
 *  interface within the device.
 */
USB_ClassInfo_HID_Device_t Mouse_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_Mouse,
				.ReportINEndpoint             =
					{
						.Address              = MOUSE_IN_EPADDR,
						.Size                 = HID_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevMouseHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevMouseHIDReportBuffer),
			},
	};

void init_work( void );

void init_work( void )
{
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

unsigned mytime = 0;
void timer_handler( void ) {

	/* Timer 処理関数の呼び出しを記述する。 アプリケーションロジックを書かない。*/
	mytime++;
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	hPipe_t pipe_id;

	SetupHardware();
	init_u16Driver();

	init_work();
	SetupHardware();

	/* time tick for 1[ms] */
	init_Timer( &timer_handler, TIMER_CLKDIV_64, 250 );
	
	initMainPipe();
	/* PIPEの受信契機、送信契機検知コールバック関数が引数 */
	pipe_id = openDataPipe( rxCallback, txCallback );

	init_UART( pipe_id );
	registPipeUART ( pipe_id );

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	GlobalInterruptEnable();
	enableInterrupt();

#ifndef MAIN_TASK_KILL_SWITCH_ENABLE
	while ( breaker == 0 )
	{
		/* driver main routine */
		cli();
		{
			appMain ( pipe_id );
		}
		sei();

		HID_Device_USBTask(&Keyboard_HID_Interface);
		HID_Device_USBTask(&Mouse_HID_Interface);
		USB_USBTask();
		
		/* debug */
#if 0
#define DEBUG_PLAYER 1
		{
			/* タスクループで100msを監視し、バッファにキーコードを書き込む */
			if ( mytime > 100 ) {
				int8_t *pad = get_paddle_addr();
				int8_t *btn = get_button_addr();
//				pad[DEBUG_PLAYER] = 1;
//				pad[DEBUG_PLAYER] = -1;
				btn[DEBUG_PLAYER] = 1;
				mytime = 0;				
			}
		}
#endif /* FEATURE DEBUG CODE */
	}
#else
	/* USB task was killed. */
	while (1) {
		// u16DriverMain();
	}
#endif /* MAIN_TASK_KILL_SWITCH_ENABLE */
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
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

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Mouse_HID_Interface);

	USB_Device_EnableSOFEvents();

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
	HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
	HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	uint8_t JoyStatus_LCL    = Joystick_GetStatus();
	uint8_t ButtonStatus_LCL = Buttons_GetStatus();

	/* Determine which interface must have its report generated */
	if (HIDInterfaceInfo == &Keyboard_HID_Interface)
	{
		USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;

		/* USB Deviceのenalble端子（PDB6）が無効ならDeviceのレポートを生成しない。 */
		*ReportSize = sizeof(USB_KeyboardReport_Data_t);
		KeyboardReport->KeyCode[0] = 0x00;
		KeyboardReport->KeyCode[1] = 0x00;
		KeyboardReport->KeyCode[2] = 0x00;
		KeyboardReport->KeyCode[3] = 0x00;
		KeyboardReport->KeyCode[4] = 0x00;
		KeyboardReport->KeyCode[5] = 0x00;
		KeyboardReport->KeyCode[6] = 0x00;
		if ( isUSBDeviceEnable() != 1 )
		{
			return true;
		} else {
			/* マスタからのUSBキー入力の問い合わせ応答生成処理 */
			/* もし、処理が必要なければ return 0; */
			int8_t i;
			int8_t idx;			
			uint8_t enterkeycode;
			int8_t *pad = get_paddle_addr();
			int8_t *btn = get_button_addr();

			/* 回転角１つごとにPlayerに割り当てられたキーコードを出力。　余った部分は0x00でPadding
			   送信し切れなかったコードは次回USBデータ通信時に送信 */
			/* キーコードは USB HID to PS/2 Scan Code Translation Table を参照 */
			idx = 1;
			for ( i = 0 ; i < PLAYERS ; i++ ) {
				if ( pad[i] != 0x00 ) {
					/* 回転方向のカウントをキー入力に変換する。 蓄積している回転角数をキーコード送出のたびに０に近づける */
					if ( pad[i] < 0 ) {
						enterkeycode = keycodemap[ i ][ KEY_MAP_RIGHT_ALLOW ]; // Right arrow
						pad[i]++;
					} else  { /* pad[i] > 0 */
						enterkeycode = keycodemap[ i ][ KEY_MAP_LEFT_ALLOW ]; // Left arrow
						pad[i]--;						
					}
					
					KeyboardReport->KeyCode[idx] = enterkeycode;
					idx++;
				}

				/* ボタン押下情報を取得し、必要があれば */
				if ( btn[i] != 0 )
				{
					KeyboardReport->KeyCode[idx] = keycodemap[ i ][ KEY_MAP_BUTTON ];
					idx++;
					/* 検知したらバッファの内容をクリアする */
					btn[i] = 0;
				}
			} /* idx の値は最大でPLAYERS*2+1。 この値になった時点の書き込みでメモリ破壊 */

		return true;
		}
	}
	else
	{
/* 本プロジェクトではマウスの応答はなし */
#if 0
		USB_MouseReport_Data_t* MouseReport = (USB_MouseReport_Data_t*)ReportData;

		/* もし、処理が必要なければ return 0; */
		if ( isUSBDeviceEnable() != 1 )
		{
			/* Do nothing. */
		} else {
			uint16_t adc;

			/* Y 座標は250固定、X座標は10bit ADC値そのままを入力 */
//			MouseReport->X = (int8_t)(get_slider_pos & 0x008f ); // debug
			adc = get_slider_pos();

#ifdef FEATURE_RASPI
			MouseReport->X = (int16_t)adc - 512;
#else
			MouseReport->X = (int16_t)adc;
#endif
			MouseReport->Y = 150;
		}

		*ReportSize = sizeof(USB_MouseReport_Data_t);
		return true;
#else /* #if 0 */
		/* do nothing */
		return true;
#endif
	}
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	if (HIDInterfaceInfo == &Keyboard_HID_Interface)
	{
		uint8_t  LEDMask   = LEDS_NO_LEDS;
		uint8_t* LEDReport = (uint8_t*)ReportData;

		if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
		  LEDMask |= LEDS_LED1;

		if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
		  LEDMask |= LEDS_LED3;

		if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
		  LEDMask |= LEDS_LED4;

		LEDs_SetAllLEDs(LEDMask);
	}
}

