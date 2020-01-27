
#include "display.h"
#include "paging.h"
#include "heap.h"
#include "heap_table.h"

extern uint32_t end;
uint32_t p_address = (uint32_t)&end;
heap_t *heap=0;

static uint32_t page_align(uint32_t address)
{
  address &= ALIGNMENT;
  address += PAGE_SIZE;
  return address;
}

// Function to sort headers by size
static uint8_t header_order(void*a, void *b)
{
    return (((meta_header_t*)a)->size < ((meta_header_t*)b)->size)?1:0;
}

uint32_t alloc_virtual(size_t size, uint8_t align)
{
  if (heap != 0)
  {
      // TODO: allocate stuff here once heap is initialised
  }
  else
  {
    if( align == 1 && (p_address & ALIGNMENT))
    {
      p_address = page_align((uint32_t)&p_address);
    }
    uint32_t mem = p_address;
    p_address += size;
    return mem;
  }
  return 0;
}

uint32_t alloc_physical(size_t size, uint8_t align, uint32_t *physical)
{
  if (heap != 0)
  {
      // TODO: allocate stuff here once heap is initialised
  } else
  {
    if( align == 1 && (p_address & ALIGNMENT))
    {
      p_address = page_align((uint32_t)&p_address);
    }
    // allocate physical memory address
    *physical = p_address;
    uint32_t mem = p_address;
    p_address += size;
    return mem;
  }
  return 0;
}

heap_t *create_heap(uint32_t start, uint32_t end, uint32_t max)
{
    heap_t *heap = (heap_t*)alloc_virtual(sizeof(heap_t), 0);
    meta_table_t heap_table = initialise_table((void*)start, HEAP_INDEX_SIZE, &header_order);

    start += sizeof(form_t)*HEAP_INDEX_SIZE;

    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;

    // make first initial hole
    meta_header_t *init_hole = (meta_header_t *)start;
    init_hole->magic = HEAP_MAGIC;
    init_hole->size = end - start;
    init_hole->free = 1;

    insert((void*)init_hole, &heap_table);

    return heap;
}
// function that finds smallest hole
uint32_t smallest_hole(uint32_t size, uint8_t page_aligned, heap_t *heap)
{
  int i;
  for(i = 0; i < (int)heap->table.size; i++)
  {
    meta_header_t *header = (meta_header_t *)search(i, &heap->table);
    if(page_aligned == 1)
    {
        // start of header needs to be page aligned.
        uint32_t header_address = (uint32_t)header;
        if((((header_address+sizeof(meta_header_t)) & ALIGNMENT) != 0))
        {
            uint32_t hole_size = ((uint32_t)header->size)
              - (PAGE_SIZE - ((header_address+sizeof(meta_header_t))%PAGE_SIZE));

            // if we find a fit then we break and return the index
            if(hole_size >= (uint32_t)size)
              break;
        }
    }
    else if (header->size >= size)
      break;
  }
  // if i is the same size as table size then nothing has been found
  if( i == (int)heap->table.size)
    return -1;
  else
    return i;
}

void alter_heap_size(uint32_t new_size, heap_t *heap)
{
  // first find out if we need to grow or shrink
  uint32_t current_size = (heap->end_address - heap->start_address);

  if ((new_size & ALIGNMENT) != 0)
  {
    new_size = page_align(new_size);
  }

  if( new_size > current_size )
  {
    // grow the heap
    for(int i = current_size; i < (int)new_size; i += PAGE_SIZE)
    {
      // TODO: allocate frame here
    }
  }
  else
  {
    // shrink the heap
    if(new_size < HEAP_MIN)
      new_size = HEAP_MIN;

    for(int i = (current_size - PAGE_SIZE); (int)new_size < i; i -= PAGE_SIZE)
    {
      // TODO: free frame here
    }
  }
    heap->end_address = heap->start_address + new_size;
}
