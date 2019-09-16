/*
 * proj.h
 *
 *  Created on: May 3, 2019
 *      Author: peter
 */

#ifndef __PROJ_H_
#define __PROJ_H_

#include "inttypes.h"

#define log(a)            RTT_print((char *) a, "[2;37m")
//#define log(a)            uart0_print((char *) a)

// event handler - interrupt sources
#define            SYS_EVH_NULL 0
#define        SYS_EVH_UART0_RX 0x1     // pmodA UART IRQ (user interface uart)

#define               UNUSED(x) (void)(x)

#endif /* __PROJ_H_ */
