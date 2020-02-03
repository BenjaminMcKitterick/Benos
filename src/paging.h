
#ifndef PAGING_H
#define PAGING_H

#define ENTRIES 1024

#include <stddef.h>
#include <stdint.h>

void enable_paging();
void initialise_paging();

typedef struct page
{
    uint32_t present        : 1;          // if 1 page in physical memory
    uint32_t rw             : 1;          // page read/write
    uint32_t user           : 1;          // priviledge level
    uint32_t wt             : 1;          // wt cache enabled
    uint32_t cache          : 1;          // if 1 then cache disabled
    uint32_t access         : 1;          // if 1 page r | w to
    uint32_t dirty          : 1;          // page has been written to
    uint32_t global         : 1;          // if 1 prevents TLB updating address
    uint32_t reserved       : 4;          // reserved bits
    uint32_t frame          : 20;         // frame address
} page_t;

typedef struct page_table
{
    page_t pages[ENTRIES];
} page_table_t __attribute__((aligned(4096)));

typedef struct page_directory
{
    page_table_t *entry[ENTRIES];
    uint32_t physical_tables[ENTRIES];
} page_directory_t __attribute__((aligned(4096)));

page_t *retrieve_page(uint32_t address, page_directory_t *dir);
void load_directory(page_directory_t *directory);

#endif
