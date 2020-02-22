#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utility.h"
#include "display.h"
#include "interrupts.h"
#include "descriptors.h"
#include "keyboard.h"
#include "paging.h"
#include "pic.h"
#include "pit.h"
#include "heap.h"

void kernel_main(void)
{
	intitialise_tables();
	install_ISRs();
	install_timer();
	install_keyboard();
	initialise_paging();

	/* After the exceptions have been registered we can re-map
	   the PIC and set the interrupt flag (sti) to allow IRQs
		 to happen. */

	pic_remap(0x20, 0x28);
	asm volatile ("sti");

	uint32_t a = malloc_virt(8,0);
	uint32_t b = malloc_virt(8,0);
	uint32_t c = malloc_virt(8,0);

	println("a %h", a);
	println("b %h", b);
	println("c %h", c);

	// not sure if deallocate works...
	deallocate(c);
	deallocate(b);
	uint32_t d = malloc_virt(12,0);
	println("d %h", d);

	while(true){
		//move_entry(0,0);
	}
}
