
#ifndef HEAP_H
#define HEAP_H

#include "heap_table.h"

#define HEAP_START          0xB0000000
#define HEAP_INDEX_SIZE     0x20000
#define HEAP_INITIAL_SIZE   0x100000
#define HEAP_MAGIC          0x0DECAF
#define HEAP_MAX            0xBFFFF000
#define HEAP_END (HEAP_START + HEAP_INITIAL_SIZE)

typedef struct
{
    uint32_t magic;           // for identification
    uint32_t size;            // size includes block footer
    uint8_t  free;            // if 1 this is free
} meta_header_t;

typedef struct
{
    uint32_t magic;           // for identification
    meta_header_t *header;   // points to block header
} meta_footer_t;

typedef struct
{
    meta_table_t table;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_address;
} heap_t;

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max);
uint32_t alloc_memory(size_t size, uint8_t align);

#endif
