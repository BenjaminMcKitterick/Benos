
#include <display.h>
#include <pit.h>
#include <scheduler.h>
#include <heap.h>
#include <thread.h>

uint32_t scheduling = 0;
uint32_t n_tasks = 0;
task_t *current_task;
task_t *previous_task;
struct task *tasks[128];

static void operation(void *data)
{
  println("%s", (char *) data);
}

static task_t *create_task(int id, void (*function)(void *), void *message)
{
  task_t *task;
  task = malloc_virt(sizeof(task_t), 0);
  task->r = malloc_virt(sizeof(reg_t), 0);
  task->function = function;
  task->message = message;
  asm volatile("mov %%cr3, %0" : "=r"(task->cr3));
  uint32_t stack = malloc_virt(PAGE_SIZE,0);
  task->stack = stack;
  task->id = id;

  return task;
}

void add_to_queue(task_t *task)
{
    n_tasks++;
    tasks[n_tasks] = task;
    if (n_tasks > 1){
      for(int i = 0; i < n_tasks; i++){
        tasks[i]->next = tasks[i+1];
      }
      tasks[n_tasks]->next = tasks[1];
    }
}

void execute_task(task_t *task)
{
    // execute task function and display task data
    task->function(task->message);
    println("id -> %h, address -> %h", task->id, task);
}

void initialise_scheduler(){

    scheduling = 1;

    task_t *task1 = create_task(1,&operation, "I am the first operation");
    task_t *task2 = create_task(2,&operation, "I am the second operation");
    task_t *task3 = create_task(3,&operation, "I am the third operation");
    task_t *task4 = create_task(4,&operation, "I am the fourth operation");

    // initialise basic round robin policy

    add_to_queue(task1);
    add_to_queue(task2);
    add_to_queue(task3);
    add_to_queue(task4);

    current_task = task1;

}

void switch_tasks(struct reg_state r)
{
    /* copy the saved register from the interrupt timer
       into the task struct */

    copy_memory(current_task->r, r, sizeof(reg_t));

    if(current_task->next != 0){
      execute_task(current_task);
      current_task = current_task->next;
    }

    /* copy the current processes register state back
       to the stack */

    copy_memory(r, &current_task->r, sizeof(reg_t));

    switch_directory(current_task->cr3);
}
