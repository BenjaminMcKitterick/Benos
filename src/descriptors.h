
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <stddef.h>

void intitialise_tables();
void gdt_init();
void idt_init();
void gdt_setup(uint32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void idt_setup(uint8_t index, uint32_t base, uint16_t selector, uint8_t type);

/*__attribute__ ((aligned (4))) specifies that the struct must
	use a data alignment of 4 bytes. Forcing the table to be aligned
	in a specific order within memory. */

struct gdt {
	uint16_t limit_low;					// lower 16 bits of limit
	uint16_t base_low;					// lower 16 bits of base
	uint8_t base_middle;				// middle 8 bits of base
	uint8_t access;							// access flags
	uint8_t granularity;        // 0 = 1byte 1 = kbyte
	uint8_t base_high;          // final 8 bits of base
} __attribute__ ((aligned (4)));

struct gdt_pointer {
	uint16_t limit;							// upper 16 bits of limit
	uint32_t base;							// address of the first gdt struct
} __attribute__ ((packed));

struct idt {
	uint16_t base_low;					// lower 16 bits of base
	uint16_t selector;					// code segment selector in the gdt
	uint8_t zero;								// unused
	uint8_t type;								// type of the description
	uint16_t base_high;					// higher 16 bits of base
} __attribute__ ((aligned (4)));

struct idt_pointer {
	uint16_t limit;							// upper 16 bits of limit
	uint32_t base;							// address of the idt struct
} __attribute__ ((packed));

/* externs for accessing assembly isr's */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* externs for accessing asm irq handlers */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif
