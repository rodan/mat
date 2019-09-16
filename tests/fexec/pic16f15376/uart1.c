#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include "proj.h"
#include "uart1.h"

uint16_t uart1_print(const char *str)
{
    return uart1_tx_str((uint8_t *) str, (uint16_t) strlen(str));
}

uint16_t uart1_tx_str(const uint8_t *str, const uint16_t size)
{
    uint16_t i;
    uint16_t bytes_txd = 0;

    for (i = 0; i < size; i++)
    {
        while (PIR3bits.TX1IF != 1) {;}
        TX1REG = str[i];
        bytes_txd++;
    }
    return bytes_txd;
}