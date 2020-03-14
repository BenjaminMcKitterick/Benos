
#ifndef HMAP_H
#define HMAP_H

#include <stdint.h>

typedef void* unknown_t;
typedef char (*order_t)(unknown_t, unknown_t);

typedef struct
{
    unknown_t *pointer;
    order_t order;
    uint32_t max_size;
    uint32_t size;
} meta_table_t;

uint8_t lessthan(unknown_t a, unknown_t b);
meta_table_t initialise_table(void *address, uint32_t max_size, order_t less_than);
void table_insert(unknown_t block, meta_table_t *table);
unknown_t table_search(uint32_t n, meta_table_t *table);
void table_delete(uint32_t i, meta_table_t *table);

#endif
