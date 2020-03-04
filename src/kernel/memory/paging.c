
#include "paging.h"
#include "heap.h"
#include "../utility/display.h"
#include "frame.h"

static void page_fault();
extern uint32_t break_point_address;
page_directory_t *kernel_directory=0;
extern heap_t *kernel_heap;

static page_directory_t *new_directory(uint32_t *physical)
{
    if (physical == 0)
    {
        page_directory_t *directory = (page_directory_t*)malloc_virt(sizeof(page_directory_t), 1);
        set_memory(directory, 0, sizeof(page_directory_t));
        return directory;
    }
    page_directory_t *directory = (page_directory_t*)malloc_phys(sizeof(page_directory_t), 1, &physical);
    set_memory(directory, 0, sizeof(page_directory_t));
    return directory;
}

void initialise_paging()
{
    nframes = PAGE_END_ADDRESS / PAGE_SIZE;
    frames = (uint32_t*)malloc_virt((nframes/BITMAP_SIZE), 1);
    set_memory(frames, 0, nframes/BITMAP_SIZE);

    kernel_directory = (page_directory_t*)malloc_virt(sizeof(page_directory_t),1);
    set_memory(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physical_address = (uint32_t)kernel_directory->physical_tables;

    page_t *heap_page;
    for (int i = HEAP_START; i < HEAP_END; i += PAGE_SIZE)
    {
        heap_page = fetch_page(i, 1, kernel_directory);
    }

    int i = 0;
    page_t *new_page;

    // Assign frames to pages
    while (i < break_point_address+PAGE_SIZE)
    {
        new_page = fetch_page(i, 1, kernel_directory);
        allocate_frame(new_page, 0, 0);

        i += PAGE_SIZE;
    }

    // allocate previously mapped pages
    for (int i = HEAP_START; i < HEAP_END; i += PAGE_SIZE)
    {
        heap_page = fetch_page(i, 1, kernel_directory);
        allocate_frame(fetch_page(i, 1, kernel_directory), 0, 0);
    }

    /* register the interrupt hander, load the directory and
       initialise the kernel heap. */

    reg_int_handler(14, page_fault);
    load_directory(kernel_directory);
    kernel_heap = initialise_heap(HEAP_START, HEAP_START+HEAP_INITIAL_SIZE, HEAP_MAX, 0, 0);
}

void load_directory(page_directory_t *directory)
{
    asm volatile("mov %0, %%cr3":: "r"(directory->physical_address));
    enable_paging();
}

page_t *fetch_page(uint32_t address, uint8_t make, page_directory_t *directory)
{
    uint32_t table_index = (address/=PAGE_SIZE)/ONE_K_BYTE;
    uint32_t pages_index = address%ONE_K_BYTE;
    page_table_t *page_table = directory->page_tables[table_index];
    if (directory->page_tables[table_index])
    {
        // If already exists then return the page
        return &page_table->pages[pages_index];
    }
    else if(make)
    {
        uint32_t base;
        directory->page_tables[table_index] = (page_table_t*)malloc_phys(sizeof(page_table_t), 1, &base);
        set_memory( directory->page_tables[table_index], 0, PAGE_SIZE);
        directory->physical_tables[table_index] = base | DIR_FLAGS;
        return &directory->page_tables[table_index]->pages[pages_index];
    }
    else
    {
        return 0;
    }
}

static void page_fault(struct reg_state reg)
{
    uint32_t fault_address;
    asm volatile("mov %%cr2, %0" : "=r" (fault_address));
    println(" Interrupt: %h Page Fault Exception {", reg.int_num);
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
    if (!p) { println("     ERROR: A page directory or table tables is not present \
                          in physical memory.");}
    if (r) { println("     ERROR: A reserved bit in a page directory or table tables \
                          is set.");}
    if (i) { println("     ERROR: Attempt to load the instruction TLB with a translation \
                          for a non-executable page.");}
    println(" ");
    println(" }");
    print_error(" Stopping system execution...");
}
