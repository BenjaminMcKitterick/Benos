
#include "thread.h"
#include "scheduler.h"
#include "../utility/display.h"

thread_t *create_thread(int id, int prio, thread_t *task, context_t *context)
{
    thread_t *thread = (thread_t *)malloc_virt(sizeof(thread_t), 0);
    thread->thread_id = id;
    thread->priority = prio;
    thread->task = task;
    thread->context = context;
    return thread;
}

void start_thread(thread_t *thread)
{
    println(" %h ", thread->task);
    println(" %h ", thread->context);
    execute_task(thread->task);
}

void kill_thread(thread_t *thread)
{
    deallocate(thread);
}
