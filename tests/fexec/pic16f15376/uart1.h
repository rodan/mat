#ifndef __UART1_H__
#define __UART1_H__

#include <stdio.h>

#define UART_CLI_BUFFER_SIZE 128

#define    UART1_EV_NULL 0
#define      UART1_EV_RX 0x1
#define      UART1_EV_TX 0x2

uint16_t uart1_print(const char *str);
uint16_t uart1_tx_str(const uint8_t *str, const uint16_t size);

#endif
