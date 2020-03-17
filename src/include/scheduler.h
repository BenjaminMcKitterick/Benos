
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <interrupts.h>
#include <heap.h>
#include <paging.h>

typedef struct task {
  uint8_t id;
  void (*function)(void *);         // function pointer
  void *message;                    // message for task to print
  uint32_t *stack;                  // pointer to its stack
  struct reg_state *r;
  struct task *next;
  uint32_t cr3;
} task_t;

void initialise_scheduler();
void switch_tasks(struct reg_state r);
void execute_task(task_t *task);

#endif
