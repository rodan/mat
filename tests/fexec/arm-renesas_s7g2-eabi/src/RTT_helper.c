/*
 * RTT_helper.c
 *
 *  Created on: 15 May 2019
 *      Author: peter
 */

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "RTT_helper.h"

#define MAX_LENGTH_OF_LOG_MESSAGE_TO_DISPLAY    200

uint8_t RTT_print(char *stringToLog, char *color)
{
    char msgToRTT[MAX_LENGTH_OF_LOG_MESSAGE_TO_DISPLAY + 15];

    msgToRTT[0] = 0;
    strncat(msgToRTT, stringToLog, MAX_LENGTH_OF_LOG_MESSAGE_TO_DISPLAY);
    SEGGER_RTT_Write(0, color, strlen(color));
    SEGGER_RTT_Write(0, msgToRTT, strlen(msgToRTT));

    return(EXIT_SUCCESS);
}
