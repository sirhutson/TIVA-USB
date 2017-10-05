#include "uart_helper.h"

//Redirect printf from Chase
void UART_OutChar(char data);
int fputc(int ch, FILE *f);


//Redirect info to debugger from SirHutson

int fgetc(FILE *f);
char Mander(void);
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMNNNNMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMN+IIIIIIINMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMNIIIII$IIIIIIMMMMMMMMMMMMMMM
//MMMMMMMMNIIIIIIIIN.NNIIIINMMMMMMMMMMMMMM
//MMMMMMMMNDZIIIIIZNNNZIIIIIMMMMMMMMMMMMMM
//MMMMMMMMD8888IIIINZZNIIII7MMMMMMMMMMMMMM
//MMMMMMMMMD88888IIIIIIIIIIDMMMMMMM8M+MMMM
//MMMMMMMMMMMD8??7$OIIIIIIIMMMMMMMM+++=MMM
//MMMMMDI7NMD8???8IIIIIIII7MMMMMMMZ++++MMM
//MMMMMIIII7$8?ZIIIIIIIII7NI+IIIM+++I777MM
//MMMMMMIIIII$NNN777II7=++IIIIIIN+++?777MM
//MMMMMMMIIIIIIN~8IIIIIIIIIIIIIDMZ+7777MMM
//MMMMMMMMOIII~~~~~NIIIIIIII7MMMMMMM7OMMMM
//MMMMMMMMMM7+~~~~~~III77DIMMMMMMMMM7IMMMM
//MMMMMMMMMMD~~~~~~~$7IIIIINMMMMMMMMIZMMMM
//MMMMMMMMMM~~~~~~~~~IIIIIIIMMMMMMMDI7MMMM
//MMMMMMMMMM~~~~~~~~~IIIIIIIMMMMMMOI77MMMM
//MMMMMMM8+$~~~~~~~~~IIIIIIIMMMMDII7~MMMMM
//MMMMMN+III?~~~~~~~87IIIIII77IIIII~MMMMMM
//MMMMMIIII77=~~~~~IIIIIIII777IIID+MMMMMMM
//M,7IIII77777+~~~7IIIIIIIIN77N++8MMMMMMMM
//MMN777I777777O++77IIIII77++++DMMMMMMMMMM
//MMMMD77777MMMMMM77777777NNMMMMMMMMMMMMMM
//MMMMMMD77MMMMMMMMN77777NMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMN77777DMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMD=7~7~MMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

//This function sets up your Uart hardware
void UART_setup()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); //Turns on the UART port
	
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Names these pins as being the UART pins
	
	GPIOPinConfigure(GPIO_PA0_U0RX); //Pin 0 is configured as receive
	
	GPIOPinConfigure(GPIO_PA1_U0TX); //Pin 1 is configured as transmit

	//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
    //                   SYSCTL_XTAL_16MHZ);   //This will be moved to a different module
	
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 38400,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));  // This was put in the UG as necessary not sure how the parameters affect the system
	
} 

char Mander()
{
	while((UART0_FR_R&UART_FR_RXFE) != 0); //waits for the receive fifo to not be empty
	return ((char)(UART0_DR_R&0xFF)); //returns the character from the port
}
// Get a character from UART.
int fgetc(FILE *f){
  return Mander();
}

// Function called when file error occurs.
int ferror(FILE *f){
  /* Your implementation of ferror */
  return 1;
}
void UART_OutChar(char data){
  while((UART0_FR_R&UART_FR_TXFF) != 0);  //waits for the send fifo to be clear
  UART0_DR_R = data;
}

// Print a character to UART.
int fputc(int ch, FILE *f){
  if((ch == 10) || (ch == 13) || (ch == 27)){
    UART_OutChar(13);
    UART_OutChar(10);
    return 1;
  }
  UART_OutChar(ch);
  return 1;
}

//This function retrieves UART data
void process_UART(uint32_t base) // Processes data for any uart port but base must be entered as UARTx_BASE
{
int f;
		
	f = UARTCharGet(base); //waits until character is put in UART and makes f equal to that
	
	printf("You sent this character over %c\n\r",f);
			
		
} 

//This function prints a menu in human readable format
void print_menu()
{
	int i;
	//char select[6];   // character array with the name select
	char select[] = {'A','B','C','D','E','F'};
	
	printf(" Please select letter to execute that function\n\r");
	
	for(i=0;i<6;i++)
	{
		printf("%c\n\r",select[i]);
	}
	
	
	// maybe create an array and have the value printed/sent to the uart port?
	//Uart must be initialized first.
	//code here.
}