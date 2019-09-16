#include <stdio.h>
#include <string.h>

#include "hal_data.h"
#include "proj.h"
#include "uart0.h"

uint8_t uart0_rx_buf[UART_CLI_BUFFER_SIZE];

volatile int uart0_rx_idx;
volatile uint8_t uart0_rx_enable;
volatile uint8_t uart0_tx_complete;
volatile uint8_t uart0_rx_complete;
volatile uint8_t uart0_last_event;

ssp_err_t uart0_init(void) {
    ssp_err_t err;
    // disable output buffering
    setvbuf (stdout, NULL, _IONBF, UART_CLI_BUFFER_SIZE);
    err = g_uart0.p_api->open(g_uart0.p_ctrl, g_uart0.p_cfg);
    uart0_rx_enable = 1;
    uart0_rx_idx = 0;
    uart0_rx_complete = 0;
    uart0_last_event = UART0_EV_NULL;
    return err;
}

uint8_t uart0_get_event(void)
{
    return uart0_last_event;
}

void uart0_rst_event(void)
{
    uart0_last_event = UART0_EV_NULL;
}

void uart0_set_eol(void)
{
    uart0_rx_idx = 0;
    uart0_rx_enable = 1;
}

void uart0_disable_rx(void)
{
    uart0_rx_enable = 0;
}

uint8_t *uart0_get_rx_buf(void)
{
    if (uart0_rx_idx) {
        return (uint8_t *)uart0_rx_buf;
    } else {
        return NULL;
    }
}

uint8_t uart0_get_rx_complete(void)
{
    return uart0_rx_complete;
}

uint16_t uart0_print(const char *str)
{
    return uart0_tx_str(str, (uint16_t) strlen(str));
}

uint16_t uart0_tx_str(const uint8_t *str, const uint16_t size)
{
    uint16_t i;
    uint16_t bytesTransmitted = 0;

    for (i = 0; i < size; i++)
    {
        uart0_tx_complete = 0;
        ssp_err_t err = g_uart0.p_api->write (g_uart0.p_ctrl, (uint8_t const *)(str + i), 1);
        if (err != SSP_SUCCESS)
        {
            break;
        }
        while (!uart0_tx_complete) {}
        bytesTransmitted++;
    }
    return bytesTransmitted;
}

ssp_err_t uart0_read(void)
{
    uart0_rx_complete = 0;
    return g_uart0.p_api->read (g_uart0.p_ctrl, NULL, (uint32_t)NULL);
    while (!uart0_rx_complete) {};
}

void uart0_callback(uart_callback_args_t * p_args)
{
    uint8_t ev = 0;
    uint8_t rx;

    // Get Event Type
    switch (p_args->event)
    {
        // Transmission Complete
        case UART_EVENT_TX_COMPLETE:
            uart0_tx_complete = 1;
        break;

        // Received Character
        case UART_EVENT_RX_CHAR:
            // Add data to input Buffer
            rx = (uint8_t) p_args->data;

            if (rx == 0x0a) {
                return;
            }

            if (uart0_rx_enable && (uart0_rx_idx < UART_CLI_BUFFER_SIZE)) {
                if (rx == 0x0d) {
                    uart0_rx_buf[uart0_rx_idx] = 0;
                    uart0_rx_enable = 0;
                    if (uart0_rx_idx) {
                        ev = UART0_EV_RX;
                        //_BIC_SR_IRQ(LPM3_bits);
                        uart0_rx_complete = 1;
                    }
                } else {
                    uart0_rx_buf[uart0_rx_idx] = rx;
                    uart0_rx_idx++;
                }
            } else {
                uart0_rx_idx = 0;
                if ((rx == 0x0d) || (rx == 0x0a)) {
                    uart0_rx_enable = 1;
                }
            }

            break;
        case UART_EVENT_RX_COMPLETE: break;
        case UART_EVENT_ERR_PARITY: break;
        case UART_EVENT_ERR_FRAMING: break;
        case UART_EVENT_BREAK_DETECT: break;
        case UART_EVENT_ERR_OVERFLOW: break;
        case UART_EVENT_ERR_RXBUF_OVERFLOW: break;
        case UART_EVENT_TX_DATA_EMPTY: break;
    }

    uart0_last_event |= ev;
}

// printf() will use this function
int _write(int file, uint8_t *buffer, int count)
{
    UNUSED(file);
    uint16_t size = (uint16_t) count;
    return uart0_tx_str(buffer, size);
}

void error_trap(ssp_err_t err, uint8_t *msg)
{
    //sigerr_on();
    //printf("error 0x%x: %s\n", err, msg);
}
