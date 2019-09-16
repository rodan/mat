#ifndef __UART0_H__
#define __UART0_H__

#include <stdio.h>
#include "hal_data.h"

#define UART_CLI_BUFFER_SIZE 128

#define    UART0_EV_NULL 0
#define      UART0_EV_RX 0x1
#define      UART0_EV_TX 0x2

ssp_err_t uart0_init(void);
int _write(int file, uint8_t *buffer, int count);
void error_trap(ssp_err_t err, uint8_t *msg);

uint8_t uart0_get_event(void);
void uart0_rst_event(void);
void uart0_set_eol(void);
void uart0_disable_rx(void);
uint8_t *uart0_get_rx_buf(void);
uint8_t uart0_get_rx_complete(void);

uint16_t uart0_print(const char *str);
uint16_t uart0_tx_str(const uint8_t *str, const uint16_t size);
ssp_err_t uart0_read(void);

void uart0_callback(uart_callback_args_t * p_args);

#endif
