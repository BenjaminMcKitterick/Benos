
#ifndef HEAP_H
#define HEAP_H

#include <utility.h>
#include <heap_table.h>

#define HEAP_START         0xB0000000
#define HEAP_INITIAL_SIZE  0x100000
#define ALIGNMENT          0xFFFFF000
#define HEAP_INDEX_SIZE    0x20000
#define HEAP_MAX           0xBFFFF000
#define HEAP_MAGIC         0xDECAF
#define HEAD_MAGIC         0xBADBED
#define FOOT_MAGIC         0xBAABAA
#define HEAP_MIN_SIZE      0x50000
#define HEAP_END (HEAP_START + 0x100000)

typedef struct
{
    uint32_t magic;                  // for identification
    uint32_t size;                   // size includes block footer
    uint8_t  free;                   // if 1 this is free
} meta_header_t;

typedef struct
{
    uint32_t magic;                  // for identification
    meta_header_t *head;             // points to block header
} meta_footer_t;

typedef struct
{
    meta_table_t table;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_address;
    uint8_t role;
    uint8_t readable;
} heap_t;

heap_t *initialise_heap(uint32_t start, uint32_t end, uint32_t max, uint8_t role, uint8_t read);
void *allocate(uint32_t size, uint8_t align, heap_t *heap);
void deallocate(void *pointer);

#endif //
