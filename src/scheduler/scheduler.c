
#include <display.h>
#include <pit.h>
#include <scheduler.h>
#include <thread.h>

context_t *current_context = 0;

static void operation(void *data)
{
  println("%s", (char *) data);
}

static task_t *create_task(void (*function)(void *), void *message)
{
  task_t *task;
  task = malloc_virt(sizeof(task_t), 0);
  task->function = function;
  task->message = message;

  // Allocate a fixed 0x1000 area of space for task stack
  uint32_t stack = malloc_virt(0x1000, 0);
  task->stack = stack;

  get_context();
  task->context = current_context;
  current_context = 0;

  println("context: %h ", task->context);

  return task;
}

// process expects a stack, copy of interrupt registers on the stack, page directory
// new stack pointer referencing memory - stack needs something to use as those registers
// seg, inst pointer. // two stack frames and something that calls exit at the end.

// Deallocate stack when exiting thread

// used fixed amount of space for stack - identity map pages first

void execute_task(task_t *task)
{
    task->function(task->message);
    //println(" %h ", task->context);
    //deallocate(task);
}

void switch_context(context_t *context)
{
    // swap all register state stuff
    // first set last process = to current process
    //println(" t1: %h ", context);
    context_switch(context);

    println("before: %h", context->useresp);
    get_context();
    println("after: %h", current_context->useresp);
    // then call an asm function to switch over
    println(" i did it woooo");
}

void context_handler(context_t c)
{
    context_t *context = (context_t*)malloc_virt(sizeof(context_t), 0);

    context->edi = c.edi;     // Destination Pointer
    context->ebx = c.ebx;     // Pointer to data in the DS segment
    context->edx = c.edx;     // IO pointer
    context->ecx = c.ecx;     // counter for string and loop
    context->eax = c.eax;     // Accumulator
    context->cs = c.cs;
    context->eflags = c.eflags;
    context->useresp = c.useresp;
    context->error_code = c.error_code;
    asm volatile("mov %%cr3, %0" : "=r"(context->cr3));

    println(" edi %h, ebx %h", context->edi, context->ebx);
    println(" edx %h, ecx %h", context->edx, context->ecx);
    println(" eax %h, cs %h", context->eax, context->cs);
    println(" efl %h, usp %h ", context->eflags, context->useresp);
    /*

    typedef struct context {
    uint32_t eax; // 0
    uint32_t ecx; // 4
    uint32_t edx; // 8
    uint32_t ebx; // 12
    uint32_t esp; // 16
    uint32_t ebp; // 20
    uint32_t esi; // 24
    uint32_t edi; // 28
    uint32_t eflags; //32
    uint32_t cr3; // 36
    uint32_t eip; //40
}context_t;
*/

    //println("eip %h", c.esp);

    current_context = context;
}

void do_something(){

    // begin by creating 2 threads that constantly switch between each other

    println("Doing Something...");

    task_t *task1 = create_task(&operation, "I am the first operation");
    task_t *task2 = create_task(&operation, "I am the second operation");

    thread_t *thread1 = create_thread(1 , 1, task1, task1->context);
    //thread_t *thread2 = create_thread(2 , 1, task2, task2->context);

    start_thread(thread1);
    //start_thread(thread2);

    switch_context(thread1->context);
    //switch_context(thread2->context);
}
