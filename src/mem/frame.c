
#include <stdint.h>
#include <paging.h>
#include <frame.h>
#include <heap.h>
#include <display.h>

extern uint32_t nframes;

static void set_frame_bit(uint32_t address, uint8_t bit)
{
  uint32_t frame_num = address/PAGE_SIZE;
  uint32_t i = frame_num/BITMAP_SIZE;
  uint32_t offset = frame_num%BITMAP_SIZE;
  if(bit){
    // OR is used to set the bit
    frames[i] |= (0x1 << offset);
  } else
  {
    // AND is used to unset the bit
    frames[i] &= ~(0x1 << offset);
  }
}

// finds the first free frame within a bitmap
static uint32_t retrieve_frame()
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

// function to allocate a frame to a page

void allocate_frame(page_t *page, uint8_t user, uint8_t rw)
{
    if(page->frame)
    {
        print_error(" Page is already allocated");
    }
    else
    {
        uint32_t index = retrieve_frame();
        if(index == (uint32_t)-1)
        {
          print_error(" There are no free frames ");
        }
        uint32_t frame_address = index * PAGE_SIZE;
        set_frame_bit(frame_address, 1);
        *page = (page_t) { .present = 1,
                           .frame = index,
                           .user = user,
                           .rw = rw
                         };
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
