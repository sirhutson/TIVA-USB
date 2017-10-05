
#include <stdint.h>
#include <stdbool.h>
#include "../inc/hw_types.h"
#include "../driverlib/usb.h"
#include "../usblib/usblib.h"
#include "../usblib/usbcdc.h"
#include "../usblib/usb-ids.h"
#include "../usblib/device/usbdevice.h"
#include "../usblib/device/usbdcdc.h"
//*****************************************************************************
//
// usb_serial_structs.h - Data structures defining this USB CDC device.
//
// Copyright (c) 2012-2016 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.3.156 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#ifndef _USB_SERIAL_STRUCTS_H_
#define _USB_SERIAL_STRUCTS_H_

//*****************************************************************************
//
// The size of the transmit and receive buffers used for the redirected UART.
// This number should be a power of 2 for best performance.  256 is chosen
// pretty much at random though the buffer should be at least twice the size of
// a maximum-sized USB packet.
//
//*****************************************************************************
#define UART_BUFFER_SIZE 256

struct Device
{
 uint16_t ui16VID;
 uint16_t ui16PID;
 uint16_t ui16MaxPowermA;
 uint8_t ui8PwrAttributes;
 tUSBCallback pfnControlCallback;
 void *pvControlCBData;
 tUSBCallback pfnRxCallback;
 void *pvRxCBData;
 tUSBCallback pfnTxCallback;
 void *pvTxCBData;
 const uint8_t *const *ppui8StringDescriptors;
 uint32_t ui32NumStringDescriptors;
tCDCSerInstance sPrivateData;	
};

extern uint32_t RxHandler(void *pvCBData, uint32_t ui32Event,
                          uint32_t ui32MsgValue, void *pvMsgData);
extern uint32_t TxHandler(void *pvi32CBData, uint32_t ui32Event,
                          uint32_t ui32MsgValue, void *pvMsgData);

extern tUSBBuffer g_sTxBuffer;
extern tUSBBuffer g_sRxBuffer;
extern tUSBDCDCDevice g_sCDCDevice;
extern uint8_t g_pui8USBTxBuffer[];
extern uint8_t g_pui8USBRxBuffer[];
extern struct Device DeviceInfo;

#endif
