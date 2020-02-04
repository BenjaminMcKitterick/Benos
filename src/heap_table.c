
#include "utility.h"
#include "display.h"
#include "heap_table.h"

uint8_t lessthan(form_t a, form_t b)
{
   return a < b ? 1:0;
}

meta_table_t initialise_table(void *address, uint32_t max_size, order_t less_than)
{
    meta_table_t table;
    table.pointer = (form_t*)address;
    println(" %h ", table.pointer);
    set_memory(table.pointer, 0, max_size*sizeof(form_t));
    table.order = less_than;
    table.max_size = max_size;
    table.size = 0;
    return table;
}

void insert(form_t block, meta_table_t *table)
{
    for(int i = 0; table->size && table->order(table->pointer[i], block); i++)
    {
         if(i == (int)table->size)
         {
             // add item to the end
             table->pointer[table->size++] = block;
         } else
         {
             // store temp val at index and new item in b_array
             form_t temp = table->pointer[i];
             table->pointer[i] = block;
             // now shift everything along once
             while( i < (int)table->size )
             {
               i++;
               form_t temp2 = table->pointer[i];
               table->pointer[i] = temp;
               temp = temp2;
             }
             table->size++;
         }
     }
}

form_t search(uint32_t n, meta_table_t *table)
{
    if( n < table->size)
    {
      return table->pointer[n];
    } else
    {
      println(" ERROR - NO HEAP DATA AT INDEX %d", n);
      return 0;
    }
}
