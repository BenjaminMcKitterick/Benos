
#include "paging.h"
#include "heap.h"
#include "display.h"
#include "frame.h"

static void page_fault();
extern uint32_t break_point_address;
page_directory_t *kernel_directory=0;
page_directory_t *current_directory=0;
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

static page_table_t *clone_table(page_table_t *src, uint32_t *physAddr)
{
    // Make a new page table, which is page aligned.
    page_table_t *table = (page_table_t*)malloc_phys(sizeof(page_table_t), 1, physAddr);
    // Ensure that the new table is blank.
    set_memory(table, 0, sizeof(page_directory_t));

    // For every entry in the table...
    int i;
    for (i = 0; i < 1024; i++)
    {
        // If the source entry has a frame associated with it...
        if (src->pages[i].frame)
        {
            // Get a new frame.
            allocate_frame(&table->pages[i]);
            // Clone the flags from source to destination.
            if (src->pages[i].present) table->pages[i].present = 1;
            if (src->pages[i].rw) table->pages[i].rw = 1;
            if (src->pages[i].user) table->pages[i].user = 1;
            if (src->pages[i].access) table->pages[i].access = 1;
            if (src->pages[i].dirty) table->pages[i].dirty = 1;
            // Physically copy the data across. This function is in process.s.
            copy_page_physical(src->pages[i].frame*0x1000, table->pages[i].frame*0x1000);
        }
    }
    return table;
}

page_directory_t *clone_directory(page_directory_t *src)
{
    uint32_t phys;
    // Make a new page directory and obtain its physical address.
    page_directory_t *dir = (page_directory_t*)malloc_phys(sizeof(page_directory_t), &phys);
    // Ensure that it is blank.
    set_memory(dir, 0, sizeof(page_directory_t));

    // Get the offset of physical_tables from the start of the page_directory_t structure.
    uint32_t offset = (uint32_t)dir->physical_tables - (uint32_t)dir;

    // Then the physical address of dir->physical_tables is:
    dir->physical_address = phys + offset;

    // Go through each page table. If the page table is in the kernel directory, do not make a new copy.
    int i;
    for (i = 0; i < 1024; i++)
    {
        if (!src->page_tables[i])
            continue;

        if (kernel_directory->page_tables[i] == src->page_tables[i])
        {
            // It's in the kernel, so just use the same pointer.
            dir->page_tables[i] = src->page_tables[i];
            dir->physical_tables[i] = src->physical_tables[i];
        }
        else
        {
            // Copy the table.
            uint32_t phys;
            dir->page_tables[i] = clone_table(src->page_tables[i], &phys);
            dir->physical_tables[i] = phys | 0x07;
        }
    }
    return dir;
}

/*
static page_table_t *copy_page_table(page_table_t *table, uint32_t *physical)
{
    page_table_t *new_table = 0;
    new_table = (page_table_t*)malloc_phys(sizeof(page_table_t), physical);
    set_memory(new_table, 0, sizeof(page_directory_t));

    int i;
    for( i = 0; i < ONE_K_BYTE; i++ )
    {
        if(!table->pages[i].frame)
          continue;
        // retrieve new frames
        //page_t *native_page = &table->pages[index];
        //page_t *copied_page = &new_table->pages[index];
        allocate_frame(&new_table->pages[i]);
        //copy_page_contents(native_page, copied_page);

        if(table->pages[i].present)  new_table->pages[i].present = 1;
        if(table->pages[i].rw)       new_table->pages[i].rw = 1;
        if(table->pages[i].user)     new_table->pages[i].user = 1;
        if(table->pages[i].access)   new_table->pages[i].access = 1;
        if(table->pages[i].dirty)    new_table->pages[i].dirty = 1;

        //copy_page_physical(table->pages[i].frame*PAGE_SIZE, new_table->pages[i].frame*PAGE_SIZE);
    }

    return new_table;
}

static page_directory_t *copy_directory(page_directory_t *native_dir)
{
    /* create a new directory and obtain the physical address by
       adding the physical base address and start of the directory
       struct. *//*

    page_directory_t *copied_directory = 0;
    uint32_t base;
    copied_directory = new_directory(base);
    uint32_t offset = (uint32_t)copied_directory->physical_tables - (uint32_t)copied_directory;
    copied_directory->physical_address = base + offset;

    int index = 0;
    while( index < ONE_K_BYTE )
    {
        // copy the native directoy page table to the new copied directory
        uint32_t base;
        copied_directory->page_tables[index] = copy_page_table(native_dir->page_tables[index], &base);
        copied_directory->physical_tables[index] = base | DIR_FLAGS;
        index++;
    }

    return copied_directory;
}

void copy_page_contents(page_t *native_page, page_t *copied_page)
{
    copied_page->present = native_page->present;
}
*/
void initialise_paging()
{
    nframes = PAGE_END_ADDRESS / PAGE_SIZE;
    frames = (uint32_t*)malloc_virt((nframes/BITMAP_SIZE), 1);
    set_memory(frames, 0, nframes/BITMAP_SIZE);

    //uint32_t phys;
    //kernel_directory = new_directory(0);
    kernel_directory = (page_directory_t*)malloc_virt(sizeof(page_directory_t),1);
    set_memory(kernel_directory, 0, sizeof(page_directory_t));
    //page_directory_t *directory = (page_directory_t*)malloc_virt(sizeof(page_directory_t), 1);
    //set_memory(directory, 0, sizeof(page_directory_t));
    kernel_directory->physical_address = (uint32_t)kernel_directory->physical_tables;

    page_t *heap_page;
    for (int i = HEAP_START; i < HEAP_END; i += PAGE_SIZE)
    {
        heap_page = fetch_page(i, kernel_directory);
    }

    int i = 0;
    page_t *new_page;

    // Assign frames to pages
    while (i < break_point_address+PAGE_SIZE)
    {
        new_page = fetch_page(i, kernel_directory);
        allocate_frame(new_page);
        i += PAGE_SIZE;
    }

    // allocate previously mapped pages
    for (int i = HEAP_START; i < HEAP_END; i += PAGE_SIZE)
    {
        heap_page = fetch_page(i, kernel_directory);
        allocate_frame( fetch_page(i, kernel_directory) );
    }

    /* register the interrupt hander, load the directory and
       initialise the kernel heap. */

    // TESTING COPYING ADDRESS SPACE
    // -------------------------------------------------------------
    //page_directory_t *cloned_dir = copy_directory(kernel_directory);

    // -------------------------------------------------------------

    reg_int_handler(14, page_fault);

    load_directory(kernel_directory);
    //switch_page_directory(kernel_directory);

    kernel_heap = initialise_heap(HEAP_START, HEAP_START+HEAP_INITIAL_SIZE, HEAP_MAX);
    //current_directory = kernel_directory;
    //current_directory = clone_directory(kernel_directory);
    //println("cur_dir: %h", current_directory);
    //switch_page_directory(current_directory);
    //load_directory(current_directory);
}

void switch_page_directory(page_directory_t *dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3":: "r"(dir->physical_address));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

void load_directory(page_directory_t *directory)
{
    asm volatile("mov %0, %%cr3":: "r"(directory->physical_address));
    enable_paging();
}

page_t *fetch_page(uint32_t address, page_directory_t *directory)
{
    uint32_t table_index = (address/=PAGE_SIZE)/ONE_K_BYTE;
    uint32_t pages_index = address%ONE_K_BYTE;
    page_table_t *page_table = directory->page_tables[table_index];
    if (directory->page_tables[table_index])
    {
        // If already exists then return the page
        return &page_table->pages[pages_index];
    }
    else
    {
        uint32_t base;
        directory->page_tables[table_index] = (page_table_t*)malloc_phys(sizeof(page_table_t), 1, &base);
        set_memory( directory->page_tables[table_index], 0, PAGE_SIZE);
        directory->physical_tables[table_index] = base | DIR_FLAGS;
        return &directory->page_tables[table_index]->pages[pages_index];
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
