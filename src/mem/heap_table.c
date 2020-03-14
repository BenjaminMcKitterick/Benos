
#include <utility.h>
#include <display.h>
#include <heap_table.h>

uint8_t lessthan(unknown_t item_one, unknown_t item_two)
{
   if (item_one < item_two){
      return 1;
   } else {
      return 0;
   }
}

meta_table_t initialise_table(void *address, uint32_t max_size, order_t less_than)
{
    meta_table_t table;
    table.pointer = (unknown_t*)address;
    set_memory(table.pointer, 0, max_size*sizeof(unknown_t));
    table.order = less_than;
    table.max_size = max_size;
    table.size = 0;
    return table;
}

void table_insert(unknown_t chunk, meta_table_t *table)
{
    uint32_t index;
    for( index = 0; index < table->size && table->order(table->pointer[index], chunk); index++){
      // do nothing... simply retrieving an index
    }
    if (index != table->size)
    {
        unknown_t temp_loc = table->pointer[index];
        table->pointer[index] = chunk;
        for(index; index < table->size; index++)
        {
          unknown_t swap = table->pointer[index];
          table->pointer[index] = temp_loc;
          temp_loc = swap;
        }
        table->size++;
    }
    else
        table->pointer[table->size++] = chunk; // append to end
}

unknown_t table_search(uint32_t n, meta_table_t *table)
{
    if( n < table->size)
    {
      return table->pointer[n];
    } else
    {
      println(" ERROR - NO HEAP DATA AT INDEX %h", n);
      return 0;
    }
}

void table_delete(uint32_t i, meta_table_t *table)
{
    for ( i; i < table->size; i++){
      table->pointer[i] = table->pointer[i+1];
    }
    table->size--;
}
