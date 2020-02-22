
#ifndef FRAME_H
#define FRAME_H

#define BITMAP_SIZE 32
#define PAGE_END_ADDRESS 0x1000000
#define PAGE_SIZE 0x1000

uint32_t *frames;
uint32_t nframes;
void allocate_frame(page_t *page);
void deallocate_frame(page_t *page);

#endif
