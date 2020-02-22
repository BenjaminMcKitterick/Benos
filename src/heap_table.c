
#include "utility.h"
#include "display.h"
#include "heap_table.h"

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
// -------------------------------------------------------------
// table insert has been edited and is an issue on BenOS main...
// -------------------------------------------------------------
void table_insert(unknown_t chunk, meta_table_t *table)
{
  /*
  for(int i = 0; table->size && table->order(table->pointer[i], block); i++)
  {
       if(i == (int)table->size)
       {
           // add item to the end
           table->pointer[table->size++] = block;
       } else
       {
           // store temp val at index and new item in b_array
           unknown_t temp = table->pointer[i];
           table->pointer[i] = block;
           // now shift everything along once
           while( i < (int)table->size )
           {
             i++;
             unknown_t temp2 = table->pointer[i];
             table->pointer[i] = temp;
             temp = temp2;
           }
           table->size++;
       }
   }
   */

    //for( int i = 0; i < )
    uint32_t iterator = 0;
    while (iterator < table->size && table->order(table->pointer[iterator], chunk))
        iterator++;
    if (iterator == table->size) // just add at the end of the array.
        table->pointer[table->size++] = chunk;
    else
    {
        unknown_t tmp = table->pointer[iterator];
        table->pointer[iterator] = chunk;
        while (iterator < table->size)
        {
            iterator++;
            unknown_t tmp2 = table->pointer[iterator];
            table->pointer[iterator] = tmp;
            tmp = tmp2;
        }
        table->size++;
    }
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
