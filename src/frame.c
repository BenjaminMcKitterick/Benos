
#include <stdint.h>

#include "paging.h"
#include "frame.h"
#include "heap.h"
#include "display.h"

extern uint32_t p_address;

uint32_t *frames;

void set_frame_bit(uint32_t address, uint8_t bit)
{
  uint32_t frame_num = address/PAGE_SIZE;
  uint32_t i = frame_num/BITMAP_SIZE;
  uint32_t offset = frame_num%BITMAP_SIZE;
  if(bit){
    // OR is used to set the bit
    frames[i] |= (0x1 << offset);
  }else
  {
    // AND is used to unset the bit
    frames[i] &= ~(0x1 << offset);
  }
}

// finds the first free frame within a bitmap
uint32_t find_free_frame()
{
    for(int i = 0; i < (int)(NUM_FRAMES/BITMAP_SIZE); i++)
    {
      for(int j = 0; j < BITMAP_SIZE; j++)
      {
        if(!(frames[i] & (0x1 << j)))
          return i*BITMAP_SIZE+j;
      }
    }
    return 0;
}

// function to allocate a frame to a page
void allocate_frame(page_t *page)
{
    if(page->frame)
    {
      print_error(" Page is already allocated to a frame ");
      return;
    } else
    {
        uint32_t index = find_free_frame();
        if(index)
        {
          uint32_t frame_address = index * PAGE_SIZE;
          set_frame_bit(frame_address, 1);
          page->present = 1;
          page->frame = index;
        } else {
          print_error(" There are no free frames ");
        }
    }
}

// function to free a frame from a page
void deallocate_frame(page_t *page)
{
    if( !(page->frame) )
    {
        print_error(" Page has no allocated frame ");
        return;
    } else
    {
      // clear the frame from the bitmap
      set_frame_bit(page->frame, 0);
      page->frame = 0;    // deallocate
    }
}
