
#include "heap.h"
#include "paging.h"
#include "frame.h"
#include "heap_table.h"

extern uint32_t end;
extern page_directory_t *kernel_directory;
uint32_t break_point_address = (uint32_t)&end;
heap_t *kernel_heap=0;

static void page_align()
{
    break_point_address &= ALIGNMENT;
    break_point_address += PAGE_SIZE;
}

uint32_t malloc_virt(size_t size, uint8_t align)
{
  if (kernel_heap != 0)
  {
      // TODO: allocate stuff here once heap is initialised
      void *address = allocate(size, align, kernel_heap);
      return (uint32_t)address;
  }
  else
  {
    if( align == 1 && (break_point_address & ALIGNMENT))
        page_align();

    uint32_t mem = break_point_address;
    break_point_address += size;
    return mem;
  }
  return 0;
}

uint32_t malloc_phys(size_t size, uint8_t align, uint32_t *physical)
{
  if (kernel_heap != 0)
  {
      void *address = allocate(size, align, kernel_heap);
      page_t *page = fetch_page((uint32_t)address, kernel_directory);
      *physical = page->frame*PAGE_SIZE + (uint32_t)address&0xFFF;
      return (uint32_t)address;
  } else
  {
    if( align == 1 && (break_point_address & ALIGNMENT))
        page_align();

    *physical = break_point_address;
    uint32_t mem = break_point_address;
    break_point_address += size;
    return mem;
  }
  return 0;
}

static void alter_heap_size(uint32_t new_size, heap_t *heap)
{
  // first find out if we need to grow or shrink
  uint32_t current_size = (heap->end_address - heap->start_address);

  if ((new_size & ALIGNMENT) != 0)
  {
      new_size &= ALIGNMENT;
      new_size += PAGE_SIZE;
  }

  if( new_size > current_size )
  {
    // grow the heap
    page_t *page;
    for(int i = current_size; i < (int)new_size; i += PAGE_SIZE)
    {
        // Allocate frame here
        page = fetch_page(heap->start_address+i, kernel_directory);
        allocate_frame(page);
        i += PAGE_SIZE;
    }
  }
  else
  {
    // shrink the heap
    if(new_size < HEAP_MIN_SIZE)
      new_size = HEAP_MIN_SIZE;

    page_t *page;;
    for(int i = (current_size - PAGE_SIZE); (int)new_size < i; i -= PAGE_SIZE)
    {
        // Free frame here
        page = fetch_page(heap->start_address+i, kernel_directory);
        deallocate_frame(page);
        i -= PAGE_SIZE;
    }
  }
    heap->end_address = heap->start_address + new_size;
}

static uint32_t get_smallest_chunk(uint32_t size, uint8_t aligned, heap_t *heap)
{
  int i;
  for(i = 0; i < (int)heap->table.size; i++)
  {
    meta_header_t *header = (meta_header_t *)table_search(i, &heap->table);
    if(aligned == 1)
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
    {
      break;
    }
  }
  // if i is the same size as table size then nothing has been found
  if( i == (int)heap->table.size)
    return -1;
  else
    return i;
}

static int header_order(void*header_one, void *meta_header_two)
{
    if (((meta_header_t*)header_one)->size < ((meta_header_t*)meta_header_two)->size)
        return 1;
    else
        return 0;
}

static meta_header_t *new_header(uint32_t address, uint32_t size, uint8_t f)
{
    meta_header_t *header = (meta_header_t *)address;
    header->magic = HEAD_MAGIC;
    header->size  = size;
    header->free  = f;
    return header;
}

static meta_footer_t *new_footer(uint32_t address, meta_header_t *header)
{
    uint32_t padded_address = (uint32_t)address - sizeof(meta_footer_t);
    meta_footer_t *footer = (meta_footer_t *)padded_address;
    footer->magic = FOOT_MAGIC;
    footer->head  = header;
    return footer;
}

static uint32_t get_endmost_header(meta_table_t *heap_table)
{
    uint32_t index = -1;
    uint32_t end_header_index = 0;
    for(int i = 0; i < heap_table->size; i++){
      uint32_t header_index = (uint32_t)table_search(i, &heap_table);
      if (header_index > end_header_index)
      {
          end_header_index = header_index;
          index = i;
      }
    }
    return index;
}

heap_t *initialise_heap(uint32_t start, uint32_t end_addr, uint32_t max)
{
    heap_t *heap = (heap_t*)malloc_virt(sizeof(heap_t), 0);
    meta_table_t heap_table = initialise_table( (void*)start, HEAP_INDEX_SIZE, &header_order);
    heap->table = heap_table;

    // Shift the start address forward to resemble where we can start putting data.
    start += sizeof(unknown_t)*HEAP_INDEX_SIZE;

    // Write the start, end and max addresses into the heap structure.
    heap->start_address = start;
    heap->end_address = end_addr;
    heap->max_address = max;

    // We start off with one large hole in the index.
    meta_header_t *chunk = new_header(start, (uint32_t)(end_addr-start), 1);
    table_insert((void*)chunk, &heap->table);

    return heap;
}

void *allocate(uint32_t size, uint8_t align, heap_t *heap)
{
    /* Here we get the size we need to allocate, taking into account the
       header and footer. We use a char var type to get a signed index
       return value for the smallest chunk. */

    uint32_t new_size = size + sizeof(meta_header_t) + sizeof(meta_footer_t);
    char iterator = get_smallest_chunk(new_size, align, heap);

    if (iterator == -1)
    {
        /* We didnt find a chunk so save old data and allocate more space for
           a new chunk. */

        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t old_end_address = heap->end_address;
        alter_heap_size(old_length+new_size, heap);
        uint32_t new_length = heap->end_address-heap->start_address;

        /* Iterate over the heap table and find the end most header */
        uint32_t index = get_endmost_header(&heap->table);

        if (index != -1)
        {
            // If we did find a header then we need to tweak the end header
            meta_header_t *header = table_search(index, &heap->table);
            header->size += new_length - old_length;
            meta_footer_t *footer = new_footer((uint32_t)header + header->size, header);
        }
        else
        {
          // If we didn't find any headers then we need to add an initial header
          uint32_t header_size = new_length - old_length;
          meta_header_t *header = new_header(old_end_address, header_size, 1);
          uint32_t footer_address = old_end_address + header->size;
          meta_footer_t *footer = new_footer(footer_address, header);
          table_insert((void*)header, &heap->table);
        }
        // After allocating more space we recurse and try again
        return allocate(size, align, heap);
    }

    meta_header_t *old_header = (meta_header_t *)table_search(iterator, &heap->table);
    uint32_t old_chunk_location = (uint32_t)old_header;
    uint32_t old_chunk_size = old_header->size;
    uint32_t chunk_padding = sizeof(meta_header_t)+sizeof(meta_footer_t);

    // if condition is met then increase size to the size of the chunk found
    if (old_chunk_size-new_size < chunk_padding)
    {
        size += old_chunk_size-new_size;
        new_size = old_chunk_size;
    }

    // If align is requested then page align and make a new chunk infront of the old chunk
    if (align && old_chunk_location&ALIGNMENT)
    {
        uint32_t new_location   = old_chunk_location + PAGE_SIZE - (old_chunk_location&0xFFF) - sizeof(meta_header_t);
        uint32_t chunk_size = PAGE_SIZE - (old_chunk_location&0xFFF) - sizeof(meta_header_t);
        meta_header_t *chunk_header = new_header(old_chunk_location, chunk_size, 1);
        meta_footer_t *chunk_footer = new_footer((uint32_t)new_location, chunk_header);
        old_chunk_location = new_location;
        old_chunk_size = old_chunk_size - chunk_header->size;
    }
    else
    {
        // Dont need the chunk so remove
        table_delete(iterator, &heap->table);
    }

    uint32_t footer_address = (old_chunk_location + sizeof(meta_header_t) + size);
    meta_header_t *block_header = new_header(old_chunk_location, new_size, 0);
    meta_footer_t *block_footer = new_footer(footer_address, block_header);

    /* If the chunk has positive size after allocating the previously new
       chunk then we need to make another new chunk */

    if (old_chunk_size - new_size > 0)
    {
        uint32_t header_address = (old_chunk_location + sizeof(meta_header_t) + size + sizeof(meta_footer_t));
        meta_header_t *chunk_header = new_header(header_address, old_chunk_size - new_size, 1);

        uint32_t footer_address = ((uint32_t)chunk_header + old_chunk_size - new_size - sizeof(meta_footer_t));
        meta_footer_t *chunk_footer = (meta_footer_t *)footer_address;

        if ((uint32_t)chunk_footer < heap->end_address)
        {
            chunk_footer->magic = FOOT_MAGIC;
            chunk_footer->head = chunk_header;
        }
        table_insert((void*)chunk_header, &heap->table);
    }

    // return the new allocated memory location
    uint32_t return_address = ((uint32_t)block_header+sizeof(meta_header_t));
    return (void *)return_address;
}

static uint32_t remove_header(meta_header_t *header){
    // search the table for the old header to remove it
    uint32_t index = 0;
    for(index; index < kernel_heap->table.size &&
        (table_search(index, &kernel_heap->table) != (void*)header); index++){
          // do nothing... simply iterating over for the index
    }
    table_delete(index, &kernel_heap->table);
}

void deallocate(void *pointer)
{
    if( pointer != 0){

        // firstly retrieve the header and footer of the pointer
        uint32_t header_address = pointer - sizeof(meta_header_t);
        meta_header_t *header = (meta_header_t*)header_address;
        char add_chunk = 1;
        header->free = 1;

        uint32_t footer_address = (uint32_t)header + header->size -sizeof(meta_footer_t);
        meta_footer_t *footer = (meta_footer_t*)footer_address;

        /* Check the magic value of the item to the left and if it is a footer
           we can overwrite with the new ones.  */

        meta_footer_t *sanity_footer = (meta_footer_t*)((uint32_t)header - sizeof(meta_footer_t));
        if (sanity_footer->magic == FOOT_MAGIC && sanity_footer->head->free)
        {
            uint32_t temp_size = header->size;
            header = sanity_footer->head;
            footer->head = header;
            header->size += temp_size;
            add_chunk = 0;
        }

        /* Check the magic value of the item to the right and if it is a header, we
           overwrite the footer to point to the new header and remove the old one
           from the heap table index. */

        meta_header_t *sanity_header = (meta_header_t*)((uint32_t)footer + sizeof(meta_footer_t));
        if(sanity_header->magic == HEAD_MAGIC && sanity_header->free)
        {
            header->size += sanity_header->size;
            uint32_t new_footer_address = ((uint32_t)sanity_header + sanity_header->size - sizeof(meta_footer_t));
            footer = (meta_footer_t*)new_footer_address;

            remove_header(sanity_header);
        }


        /* Now see if the footer location is the end address */
        uint32_t footer_location = (uint32_t)footer+sizeof(meta_footer_t);
        if(footer_location == kernel_heap->end_address)
        {
            uint32_t old_length = kernel_heap->end_address-kernel_heap->start_address;
            alter_heap_size((uint32_t)header - kernel_heap->start_address, kernel_heap);
            uint32_t new_length = (kernel_heap->end_address - kernel_heap->start_address);
            uint32_t difference = (old_length - new_length);

            /* Now after resizing we check to see if the we need to remove the sanity header
               or resize instead. */

            if(!(header->size - difference) > 0)
            {
                remove_header(sanity_header);
            }
            else
            {
                header->size -= difference;
                uint32_t footer_address = (uint32_t)header + header->size -sizeof(meta_footer_t);
                footer = new_footer(footer_address, header);
            }
        }

        if (add_chunk == 1)
            table_insert((void*)header, &kernel_heap->table);
    }
    else
      return;
}
