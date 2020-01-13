
#include <stdint.h>
#include <stddef.h>
#include "descriptors.h"
#include "interrupts.h"
#include "utility.h"
#include "display.h"
#include "pic.h"

#define GDT_NUM 3
#define INTVECT_NUM 256

struct gdt gdt[GDT_NUM];
struct idt idt[INTVECT_NUM];
struct gdt_pointer gdt_pointer;
struct idt_pointer idt_pointer;

extern isr int_handlers[];
static void gdt_init();
static void idt_init();

/* Initialises both the gdt and idt tables */
void intitialise_tables(){

	gdt_init();
	idt_init();

	set_memory(&int_handlers, 0, sizeof(isr)*256);

}

static void gdt_init()
{
	/* Sets the gdt pointer to allow the processor to find it. */
	gdt_pointer.limit = (sizeof(gdt) * GDT_NUM) - 1;
	gdt_pointer.base = (uint32_t)&gdt;


	gdt_setup(0, 0, 0, 0, 0);								 // null segment
	gdt_setup(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // code segment
	gdt_setup(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // data segment

	load_gdt();
}

/* Initialises an array of flags and bit values  */
void gdt_setup(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	/* set the descriptor base */
	gdt[index].base_low = base & 0xFFFF;
	gdt[index].base_middle = (base >> 16) & 0xFF;
	gdt[index].base_high = (base >> 24) & 0xFF;

	/* set the descripor limits */
	gdt[index].limit_low = limit & 0xFFFF;
	gdt[index].granularity = (limit >> 16) & 0x0F;

	/* set up the gran & access flags */
	gdt[index].granularity |= gran & 0xF0;
	gdt[index].access = access;
}

static void idt_init()
{
	idt_pointer.limit = (uint16_t)(sizeof(idt) * INTVECT_NUM -1);
	idt_pointer.base = (uint32_t)&idt;

	/* set everything in idt to zeros */
	set_memory(&idt, 0, sizeof(idt)*INTVECT_NUM);

  pic_remap(0x20, 0x28);

	idt_setup(0, (uint32_t)isr0, 0x08, 0x8E);
	idt_setup(1, (uint32_t)isr1, 0x08, 0x8E);
	idt_setup(2, (uint32_t)isr2, 0x08, 0x8E);
	idt_setup(3, (uint32_t)isr3, 0x08, 0x8E);
	idt_setup(4, (uint32_t)isr4, 0x08, 0x8E);
	idt_setup(5, (uint32_t)isr5, 0x08, 0x8E);
	idt_setup(6, (uint32_t)isr6, 0x08, 0x8E);
	idt_setup(7, (uint32_t)isr7, 0x08, 0x8E);
	idt_setup(8, (uint32_t)isr8, 0x08, 0x8E);
	idt_setup(9, (uint32_t)isr9, 0x08, 0x8E);
	idt_setup(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_setup(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_setup(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_setup(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_setup(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_setup(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_setup(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_setup(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_setup(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_setup(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_setup(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_setup(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_setup(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_setup(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_setup(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_setup(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_setup(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_setup(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_setup(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_setup(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_setup(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_setup(31, (uint32_t)isr31, 0x08, 0x8E);

	idt_setup(32, (uint32_t)irq0, 0x08, 0x8E);
  idt_setup(33, (uint32_t)irq1, 0x08, 0x8E);
  idt_setup(34, (uint32_t)irq2, 0x08, 0x8E);
  idt_setup(35, (uint32_t)irq3, 0x08, 0x8E);
  idt_setup(36, (uint32_t)irq4, 0x08, 0x8E);
  idt_setup(37, (uint32_t)irq5, 0x08, 0x8E);
  idt_setup(38, (uint32_t)irq6, 0x08, 0x8E);
  idt_setup(39, (uint32_t)irq7, 0x08, 0x8E);
  idt_setup(40, (uint32_t)irq8, 0x08, 0x8E);
  idt_setup(41, (uint32_t)irq9, 0x08, 0x8E);
  idt_setup(42, (uint32_t)irq10, 0x08, 0x8E);
  idt_setup(43, (uint32_t)irq11, 0x08, 0x8E);
  idt_setup(44, (uint32_t)irq12, 0x08, 0x8E);
  idt_setup(45, (uint32_t)irq13, 0x08, 0x8E);
  idt_setup(46, (uint32_t)irq14, 0x08, 0x8E);
  idt_setup(47, (uint32_t)irq15, 0x08, 0x8E);

	load_idt();

}

void idt_setup(uint8_t index, uint32_t base, uint16_t selector, uint8_t type)
{
	/* set the idt base */
	idt[index].base_low = base & 0xFFFF;
	idt[index].base_high = (base >> 16) & 0xFFFF;

	/* set segment selector and type */
	idt[index].selector = selector;
	idt[index].zero = 0;
	idt[index].type = type;
}
