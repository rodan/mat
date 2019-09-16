/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    // set RC6 as TX1
    ANSELCbits.ANSC6 = 0; // set as digital
    TRISCbits.TRISC6 = 0; // set as output
    RC6PPS = 0x0f; // set pin as TX1

    // set UART speed to 38400
    SP1BRGL = 52;
    SP1BRGH = 0;
    TX1STAbits.BRGH = 1; //High speed
    BAUD1CONbits.BRG16 = 0; //8-bit Baud Rate Generator
    
    RC1STAbits.SPEN = 1; //enabled
    TX1STAbits.SYNC = 0; //Asynchronous
    TX1STAbits.TX9 = 0; //8-bit transmission
    TX1STAbits.TXEN = 1; //Transmit enabled
}

