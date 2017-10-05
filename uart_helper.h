#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "sysctl.h"
#include "gpio.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_gpio.h"
#include "uart.h"
#include "pin_map.h"
#include "../inc/tm4c123gh6pm.h"

void UART_setup(void);
void process_UART(uint32_t base);
void print_menu(void);
