
#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stddef.h>
#include <stdint.h>

#define MASTER_PIC		  	0x20
#define SLAVE_PIC		     	0xA0
#define END_OF_INTERRUPT	0x20

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

struct reg_state
{	unsigned int ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_num, error_code;
	unsigned int eip, cs, eflags, useresp, ss;
} __attribute__ ((aligned(4)));

void install_ISRs();
typedef void (*isr)(struct reg_state);
void reg_int_handler(uint8_t n, isr handler);

#endif
