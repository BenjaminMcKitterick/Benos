
#ifndef PAGING_H
#define PAGING_H

void initialise_paging();
static void page_fault();
extern void enable_paging();

typedef struct page
{
    uint32_t present        : 1;
    uint32_t rw             : 1;
    uint32_t user           : 1;
    uint32_t wt             : 1;
    uint32_t cache          : 1;
    uint32_t access         : 1;
    uint32_t dirty          : 1;
    uint32_t global         : 1;
    uint32_t reserved       : 4;
    uint32_t frame          : 20;
} page_t;

typedef struct page_table
{
    page_t table_entries[1024];
} page_table_t __attribute__((aligned(4096)));

/*
   This page_dir struct consists of an array (*p_tables) to
   hold pointers to the page tables. physical_tables is another
   array of pointers for the physical address of the page tables
   for loding the CR3 register.

   note: very important that this address be 4-KiB aligned.
*/
typedef struct page_dir
{
    page_table_t *p_tables[1024];
    uint32_t physical_tables[1024];
    uint32_t physical_address;
} page_dir_t __attribute__((aligned(4096)));

#endif
