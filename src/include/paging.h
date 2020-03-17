
#ifndef PAGING_H
#define PAGING_H

#include <utility.h>
#include <interrupts.h>

#define ONE_K_BYTE  1024
#define FOUR_K_BYTE 4096
#define PAGE_SIZE   0x1000
#define DIR_FLAGS   0x7
#define BITMAP_SIZE 32

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
    page_t pages[ONE_K_BYTE];
} page_table_t __attribute__((aligned(FOUR_K_BYTE)));

typedef struct page_directory
{
    page_table_t *page_tables[ONE_K_BYTE];
    uint32_t physical_tables[ONE_K_BYTE];
    uint32_t physical_address;
} page_directory_t __attribute__((aligned(FOUR_K_BYTE)));

void enable_paging();
void initialise_paging();
void switch_directory(void* cr3);
page_t *fetch_page(uint32_t address, uint8_t make, page_directory_t *dir);

#endif
