
#ifndef PIC_REMAPPER_H
#define PIC_REMAPPER_H

void pic_remap(int master_offset, int slave_offset);
void irq_mask(uint8_t request_line);
void irq_unmask(uint8_t request_line);
uint16_t get_irq_register(int ocw3);

#endif