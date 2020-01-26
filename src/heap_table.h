
#ifndef HMAP_H
#define HMAP_H

#include <stdint.h>

typedef void* form_t;
typedef uint8_t (*order_t)(form_t, form_t);

typedef struct
{
    form_t *pointer;
    order_t order;
    uint32_t max_size;
    uint32_t size;
} meta_table_t;

uint8_t lessthan(form_t a, form_t b);

//meta_table_t create_table(uint32_t max_size, order_t less_than);
meta_table_t initialise_table(void *address, uint32_t max_size, order_t less_than);
void insert(form_t block, meta_table_t *table);

#endif
