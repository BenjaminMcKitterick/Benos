
#ifndef PAGING_H
#define PAGING_H

#define ENTRIES 1024

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
    page_t *entry[ENTRIES];
} page_table_t __attribute__((aligned(4096)));

typedef struct page_dir_entry
{
    uint32_t present          : 1;         // if 1 page is in physical memory
    uint32_t rw               : 1;         // if 1 rw
    uint32_t user             : 1;         // priviledge level
    uint32_t wt               : 1;         // if 1 wt cache enabled
    uint32_t cache            : 1;         // if 1 cache disabled
    uint32_t access           : 1;         // has been read or written to
    uint32_t page_size        : 1;         // 0 for 4kb
    uint32_t Reserved         : 5;         // reserved bits
    uint32_t table_address    : 20;        // page table address
} page_dir_entry_t;

typedef struct page_directory
{
    page_dir_entry_t *entry[ENTRIES];
} page_directory_t __attribute__((aligned(4096)));

#endif
