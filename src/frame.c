
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
uint32_t find_free_frame(uint32_t nframes)
{
    for(int i = 0; i < (int)(nframes/BITMAP_SIZE); i++)
    {
      for(int j = 0; j < BITMAP_SIZE; j++)
      {
        if(!(frames[i] & (0x1 << j)))
          return i*BITMAP_SIZE+j;
      }
    }
    return 0;
}
