#include "project.h"
int main()
{
// Enable the UART0 module.
//
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
//
// Wait for the UART0 module to be ready.
//
while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
{
}
UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 38400,
(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
UART_CONFIG_PAR_NONE));
//
// Check for characters. Spin here until a character is placed
// into the receive FIFO.
//
while(!UARTCharsAvail(UART0_BASE))
{
}
//
// Get the character(s) in the receive FIFO.
//
while(UARTCharGetNonBlocking(UART0_BASE))
{
}
//
// Put a character in the output buffer.
//
UARTCharPut(UART0_BASE, 'c');

UARTDisable(UART0_BASE);

}