
#include "paging.h"
#include "heap.h"
#include "display.h"
#include "interrupts.h"
#include "utility.h"
#include "frame.h"

#include <stddef.h>
#include <stdint.h>

page_directory_t *directory=0;
uint32_t num_frames;
static void page_fault();
extern uint32_t p_address;
extern heap_t *heap;

void initialise_paging()
{
    // Initialise memory for frames and directory
    num_frames = PAGE_END_ADDRESS/PAGE_SIZE;
    frames = (uint32_t*)malloc_virt((num_frames/BITMAP_SIZE), 0);
    directory = (page_directory_t*)malloc_virt(sizeof(page_directory_t), 1);
    set_memory(frames, 0, (num_frames/BITMAP_SIZE));
    set_memory(directory, 0, sizeof(page_directory_t));

    /* need to identity map pages to heap but not allowed to change
       p_address between identity map and heap intialisation */

    page_t *heap_page;
    for (int i = HEAP_START; i < HEAP_END; i += PAGE_SIZE)
    {
        heap_page = retrieve_page(i, directory);
    }

    int i = 0;
    page_t *new_page;

    // Assign frames to pages
    while (i < p_address+PAGE_SIZE)
    {
        new_page = retrieve_page(i, directory);
        allocate_frame(new_page);
        i += PAGE_SIZE;
    }

    // allocate previously mapped pages
    for (int i = HEAP_START; i < HEAP_END; i += PAGE_SIZE)
    {
        heap_page = retrieve_page(i, directory);
        allocate_frame( retrieve_page(i, directory) );
    }

    // Register the page fault handler then load the directory
    reg_int_handler(14, &page_fault);
    load_directory(directory);

    // Create the heap
    heap = create_heap(HEAP_START, HEAP_END, HEAP_MAX);
}

void load_directory(page_directory_t *directory)
{
    asm volatile("mov %0, %%cr3":: "r"(&directory->physical_tables));
    enable_paging();
}

page_t *retrieve_page(uint32_t address, page_directory_t *directory)
{
    uint32_t table_index = (address/=PAGE_SIZE)/ENTRIES;
    page_table_t *page_table = directory->entry[table_index];
    if (directory->entry[table_index])
    {
        // if already exists then return the page
        return &page_table->pages[address%ENTRIES];
    }
    else
    {
        uint32_t temp_addr;
        directory->entry[table_index] = (page_table_t*)malloc_phys(sizeof(page_table_t), 1, &temp_addr);
        set_memory( directory->entry[table_index], 0, PAGE_SIZE);
        directory->physical_tables[table_index] = temp_addr | 0x5;
        return &directory->entry[table_index]->pages[address%ENTRIES];
    }
}

static void page_fault(struct reg_state reg)
{
    uint32_t fault_address;
    asm volatile("mov %%cr2, %0" : "=r" (fault_address));
    println(" Interrupt: %h Page Fault Exception {", reg.int_num);
    //println("     Error code %h", reg.error_code);
    println("     Error occured at: %h, code: %h", fault_address, reg.error_code);

    uint8_t fault = reg.error_code;
    // check the error code with fault flags
    uint8_t p = fault & 0x1;
    uint8_t w = fault & 0x2;
    uint8_t u = fault & 0x4;
    uint8_t r = fault & 0x8;
    uint8_t i = fault & 0x10;

    uint32_t condition;
    if (w | u) { condition = "FAIL:"; } else { condition = "PASS"; }
    println("     Protection check: %s ", condition);
    if (w) { println("         - read/write");}
    if (u) { println("         - priviledge");}
    if (!p) { println("     ERROR: A page directory or table entry is not present \
                          in physical memory.");}
    if (r) { println("     ERROR: A reserved bit in a page directory or table entry \
                          is set.");}
    if (i) { println("     ERROR: Attempt to load the instruction TLB with a translation \
                          for a non-executable page.");}
    println(" ");
    println(" }");
    print_error(" Stopping system execution...");
}
