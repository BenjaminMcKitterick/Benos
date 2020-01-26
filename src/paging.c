
#include <stddef.h>
#include <stdint.h>

#include "paging.h"
#include "display.h"
#include "interrupts.h"
#include "heap.h"

static void page_fault();
extern heap_t *heap;
uint32_t dir_address;
uint32_t page_address;
page_directory_t page_directory[ENTRIES];
page_table_t page_table[ENTRIES];
page_dir_entry_t dir_entry[ENTRIES];
page_t page[ENTRIES];

void initialise_paging()
{
    /* create a blank page directory and set each entry to
       not present so that if the MMU looks for that page table,
       it will see that it is not there. */

    *dir_entry = (page_dir_entry_t) { .rw = 1, .user = 1};

    for(int i = 0; i < ENTRIES; i++){
      page_directory->entry[i] = dir_entry;
    }

    /* create a start default page table and set default pages */

    *page = (page_t) { .present = 1, .user = 1, .rw = 1};

    for(int i = 0; i < ENTRIES; i++){
        page_table->entry[i] = page;
    }

    /* put the page table in the directory */

    *dir_entry = (page_dir_entry_t) { .rw = 1,
       .user = 1, .table_address = &page_table};

    page_directory->entry[0] = dir_entry;
    dir_address = page_directory->entry[0]->table_address;
    page_address = page_table->entry[0]->address;
    /* enable paging using asm function */

    //enable_paging();
    reg_int_handler(14, &page_fault);

    // create heap structure
    heap = create_heap(HEAP_START, HEAP_END, HEAP_MAX);
}

static void page_fault()
{
    println("page fault triggered");
}
