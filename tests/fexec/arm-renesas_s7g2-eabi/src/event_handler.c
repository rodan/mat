
#include <inttypes.h>
#include <stdlib.h>
#include "event_handler.h"

struct event_handler *evh;

struct event_handler * event_handler_getp(void)
{
    return evh;
}

void event_handler_register(void (*callback) (const uint16_t evid),
                             const uint16_t listens)
{
    struct event_handler **p = &evh;

    while (*p) {
        p = &(*p)->next;
    }

    *p = (struct event_handler*)malloc(sizeof(struct event_handler));

    (*p)->next = NULL;
    (*p)->fn = callback;
    (*p)->listens = listens;
}

void event_handler_unregister(void (*callback) (const uint16_t evid))
{
    struct event_handler *p = evh, *pp = NULL;

    while (p) {
        if (p->fn == callback) {
            if (!pp)
                evh = p->next;
            else
                pp->next = p->next;

            free(p);
        }

        pp = p;
        p = p->next;
    }
}
