
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

void context_switch();

typedef struct context {
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int offset, int_num, error_code;
    unsigned int eip, cs, eflags, useresp, ss;
    unsigned int cr3;
} context_t __attribute__ ((aligned(4)));

typedef struct task {
  void (*function)(void *);         // function pointer
  void *message;                    // message for task to print
  uint32_t *stack;                  // pointer to its stack
  context_t *context;
} task_t;

/*
typedef struct context {
    /* 0x04 uint32_t ebx;     // Base register
    /* 0x08 uint32_t edi;     // Destination index
    /* 0x0c uint32_t esi;     // Source index
    /* 0x10 uint32_t ebp;     // Stack pointer
    /* 0x14 uint32_t esp;     // Stack Base pointer
    /* 0x18 uint32_t eflags;  // Eflags
} context_t;
*/

void do_something();
void execute_task(task_t *task);
void switch_context(context_t *context);

#endif
