#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "proj.h"

/*!
	\brief List of possible message types for the message bus.
	\sa event_handler_register()
*/

// should be re-created in local proj.h

//#define           SYS_EVH_NULL 0
//#define    SYS_EVH_TIMER0_CRR1 0x1   // timer_a0_delay_noblk_ccr1
//#define    SYS_EVH_TIMER0_CRR2 0x2   // timer_a0_delay_noblk_ccr2
//#define     SYS_EVH_TIMER0_IFG 0x4   // timer0 overflow
//#define       SYS_EVH_UART0_RX 0x8   // UART received something

/*!
	\brief Linked list of nodes listening to the message bus.
*/
struct event_handler {
    /*! callback for receiving messages from the system bus */
    void (*fn) (const uint16_t evid);
    /*! bitfield of message types that the node wishes to receive */
    uint16_t listens;
    /*! pointer to the next node in the list */
    struct event_handler *next;
};

/*!
	\brief Registers a new event.
	\details Registers (adds) a node to the event handler linked list. A node can filter what message(s) are to be received by setting the bitfield \b listens.
	\sa event_handler, event_handler_unregister
*/
void event_handler_register(
        // callback to receive messages from the event handler
        void (*callback) (const uint16_t evid),
        // only receive messages of this type
        const uint16_t listens
    );

/*!
	\brief Unregisters a node from the event handler.
	\sa event_handler_register
*/
void event_handler_unregister(
        // the same callback used on event_handler_register()
        void (*callback) (const uint16_t evid)
    );

struct event_handler * event_handler_getp(void);

#ifdef __cplusplus
}
#endif

#endif
