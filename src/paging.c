
#include "paging.h"
#include "heap.h"
#include "display.h"
#include "interrupts.h"
#include "utility.h"
#include "frame.h"

#include <stddef.h>
#include <stdint.h>

page_directory_t *directory=0;
static void page_fault();
extern uint32_t p_address;

void initialise_paging()
{
    // Initialise memory for frames and directory
    frames = (uint32_t*)malloc_virt((NUM_FRAMES/BITMAP_SIZE), 0);
    directory = (page_directory_t*)malloc_virt(sizeof(page_directory_t), 1);
    set_memory(frames, 0, (NUM_FRAMES/BITMAP_SIZE));
    set_memory(directory, 0, sizeof(page_directory_t));

    int i = 0;
    page_t *new_page;
    // Assign frames to pages
    while ((uint32_t)i < p_address)
    {
        new_page = retrieve_page(i, directory);
        allocate_frame(new_page);
        i += PAGE_SIZE;
    }

    // Register the page fault handler then load the directory
    reg_int_handler(14, &page_fault);
    load_directory(directory);
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
        set_memory(page_table, 0, PAGE_SIZE);
        directory->entry[table_index] = (page_table_t*)malloc_phys(sizeof(page_table_t), 1, &temp_addr);
        directory->physical_tables[table_index] = temp_addr | 0x3;
        return &directory->entry[table_index]->pages[address%PAGE_SIZE];
    }
}

static void page_fault(struct reg_state r)
{
    uint32_t fault_address;
    asm volatile("mov %%cr2, %0" : "=r" (fault_address));
    println(" Interrupt: %h Page Fault Exception", r.int_num);
    println(" Error %h at %h", r.offset, fault_address);
    print_error(" Stopping...");
}
