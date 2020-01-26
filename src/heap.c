
#include "display.h"
#include "paging.h"
#include "heap.h"
#include "heap_table.h"

extern uint32_t end;
uint32_t p_address = (uint32_t)&end;
heap_t *heap=0;

uint32_t alloc_memory(size_t size, uint8_t align)
{
  if (heap != 0)
  {
      // allocate stuff here once heap is initialised
  }
  else
  {
    // align the address
    if( align == 1 && (p_address & 0xFFFFF000))
    {
      p_address &= 0xFFFFF000;
      p_address += 0x1000;
    }
    uint32_t mem = p_address;
    p_address += size;
    return mem;
  }
}

static uint8_t header_order(void*a, void *b)
{
    return (((meta_header_t*)a)->size < ((meta_header_t*)b)->size)?1:0;
}

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max)
{
    heap_t *heap = (heap_t*)alloc_memory(sizeof(heap_t), 0);
    meta_table_t heap_table = initialise_table((void*)start, HEAP_INDEX_SIZE, &header_order);

    start += sizeof(form_t)*HEAP_INDEX_SIZE;

    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;

    // make first initiale hole
    meta_header_t *init_hole = (meta_header_t *)start;
    init_hole->magic = HEAP_MAGIC;
    init_hole->size = end - start;
    init_hole->free = 1;

    println("%h", heap->start_address);
    println("%h", heap->end_address);
    println("%h", heap->max_address);

    insert((void*)init_hole, &heap_table);

    return heap;
}
