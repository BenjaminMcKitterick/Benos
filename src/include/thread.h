
#ifndef THREAD_H
#define THREAD_H

#include <scheduler.h>

typedef struct thread {

    uint8_t       thread_id;
    uint8_t       priority;

    uint8_t       active;
    uint8_t       ready;
    uint8_t       waiting;

    uint32_t      time_running;
    task_t        *task;

    //context_t     *context;

} thread_t;

#endif
