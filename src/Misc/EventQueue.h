#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <stdint.h>

typedef struct {
    uint32_t frame;
    void *put_this_nonsense_into_commandblock_maybe;
} timed_event;

class EventQueue
{
public:
    EventQueue();
};

#endif // EVENTQUEUE_H
