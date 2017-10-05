#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "inc/hw_memmap.h"


#include "driverlib/adc.h"
#include "driverlib/aes.h"
#include "driverlib/can.h"
#include "driverlib/comp.h"
#include "driverlib/cpu.h"
#include "driverlib/crc.h"
#include "driverlib/debug.h"
#include "driverlib/des.h"
#include "driverlib/eeprom.h"
#include "driverlib/emac.h"
#include "driverlib/epi.h"
#include "driverlib/flash.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/lcd.h"
#include "driverlib/mpu.h"
#include "driverlib/onewire.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/qei.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/rtos_bindings.h"
#include "driverlib/shamd5.h"
#include "driverlib/ssi.h"
#include "driverlib/sw_crc.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysexc.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/usb.h"
#include "driverlib/watchdog.h"

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "./src/gpio_helper.h"
#include "./src/uart_helper.h"
#include "../src/adc_helper.h"
#include "./src/PWM.h"
#include "../inc/tm4c123gh6pm.h"
#include "./src/usb_helper.h"
#include "./src/utilsuart.h"



//*****************************************************************************
//
// usb_dev_serial.c - Main routines for the USB CDC serial example.
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



#include "../usblib/usblib.h"
#include "../usblib/usbcdc.h"
#include "../usblib/usb-ids.h"
#include "../usblib/device/usbdevice.h"
#include "../usblib/device/usbdcdc.h"
#include "usb_serial_struct.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>USB Serial Device (usb_dev_serial)</h1>
//!
//! This example application turns the evaluation kit into a virtual serial
//! port when connected to the USB host system.  The application supports the
//! USB Communication Device Class, Abstract Control Model to redirect UART0
//! traffic to and from the USB host system.
//!
//! Assuming you installed TivaWare C Series in the default directory, a
//! driver information (INF) file for use with Windows XP, Windows Vista and
//! Windows7 can be found in C:/ti/TivaWare-for-C-Series/windows_drivers.  For
//! Windows 2000, the required INF file is in
//! C:/ti/TivaWare-for-C-Series/windows_drivers/win2K.
//
//*****************************************************************************

//*****************************************************************************
//
// Note:
//
// This example is intended to run on Tiva C Series evaluation kit hardware
// where the UARTs are wired solely for TX and RX, and do not have GPIOs
// connected to act as handshake signals.  As a result, this example mimics
// the case where communication is always possible.  It reports DSR, DCD
// and CTS as high to ensure that the USB host recognizes that data can be
// sent and merely ignores the host's requested DTR and RTS states.  "TODO"
// comments in the code indicate where code would be required to add support
// for real handshakes.
//
//*****************************************************************************

//*****************************************************************************
//
// Configuration and tuning parameters.
//
//*****************************************************************************

//*****************************************************************************
//
// The system tick rate expressed both as ticks per second and a millisecond
// period.
//
//*****************************************************************************
#define SYSTICKS_PER_SECOND 100
#define SYSTICK_PERIOD_MS (1000 / SYSTICKS_PER_SECOND)

//*****************************************************************************
//
// Variables tracking transmit and receive counts.
//
//*****************************************************************************
volatile uint32_t g_ui32UARTTxCount = 0;
volatile uint32_t g_ui32UARTRxCount = 0;
#ifdef DEBUG
uint32_t g_ui32UARTRxErrors = 0;
#endif

//*****************************************************************************
//
// The base address, peripheral ID and interrupt ID of the UART that is to
// be redirected.
//
//*****************************************************************************

//*****************************************************************************
//
// Defines required to redirect UART0 via USB.
//
//*****************************************************************************
#define USB_UART_BASE           UART0_BASE
#define USB_UART_PERIPH         SYSCTL_PERIPH_UART0
#define USB_UART_INT            INT_UART0

//*****************************************************************************
//
// Default line coding settings for the redirected UART.
//
//*****************************************************************************
#define DEFAULT_BIT_RATE        115200
#define DEFAULT_UART_CONFIG     (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | \
                                 UART_CONFIG_STOP_ONE)

//*****************************************************************************
//
// GPIO peripherals and pins muxed with the redirected UART.  These will depend
// upon the IC in use and the UART selected in USB_UART_BASE.  Be careful that
// these settings all agree with the hardware you are using.
//
//*****************************************************************************

//*****************************************************************************
//
// Defines required to redirect UART0 via USB.
//
//*****************************************************************************
#define TX_GPIO_BASE            GPIO_PORTA_BASE
#define TX_GPIO_PERIPH          SYSCTL_PERIPH_GPIOA
#define TX_GPIO_PIN             GPIO_PIN_1

#define RX_GPIO_BASE            GPIO_PORTA_BASE
#define RX_GPIO_PERIPH          SYSCTL_PERIPH_GPIOA
#define RX_GPIO_PIN             GPIO_PIN_0

//*****************************************************************************
//
// Flag indicating whether or not we are currently sending a Break condition.
//
//*****************************************************************************
static bool g_bSendingBreak = false;

//*****************************************************************************
//
// Global system tick counter
//
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount = 0;

//*****************************************************************************
//
// Flags used to pass commands from interrupt context to the main loop.
//
//*****************************************************************************
#define COMMAND_PACKET_RECEIVED 0x00000001
#define COMMAND_STATUS_UPDATE   0x00000002

volatile uint32_t g_ui32Flags = 0;
char *g_pcStatus;

//*****************************************************************************
//
// Global flag indicating that a USB configuration has been set.
//
//*****************************************************************************
static volatile bool g_bUSBConfigured = false;

//*****************************************************************************
//
// Internal function prototypes.
//
//*****************************************************************************
static void USBUARTPrimeTransmit(uint32_t ui32Base);
static void CheckForSerialStateChange(const tUSBDCDCDevice *psDevice,
                                      int32_t i32Errors);
static void SetControlLineState(uint16_t ui16State);
static bool SetLineCoding(tLineCoding *psLineCoding);
static void GetLineCoding(tLineCoding *psLineCoding);
static void SendBreak(bool bSend);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1)
    {
    }
}
#endif

//*****************************************************************************

