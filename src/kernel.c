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

	//asm volatile ("int $0x8");

	// TEST PAGE FAULT
	uint32_t *ptr = (uint32_t*)0xA0000000;
  uint32_t do_page_fault = *ptr;
	println("%h", do_page_fault);

	while(true){
		//move_entry(0,0);
	}
}
