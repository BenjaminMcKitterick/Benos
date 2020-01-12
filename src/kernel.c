#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utility.h"
#include "display.h"
#include "descriptors.h"
#include "keyboard.h"
#include "paging.h"
#include "pic.h"
#include "pit.h"

void kernel_main(void)
{
	intitialise_tables();
	install_ISRs();
	install_timer();
	install_keyboard();
	//initialise_paging();

	/* After the exceptions have been registered we can re-map
	   the PIC and set the interrupt flag (sti) to allow IRQs
		 to happen. */

	pic_remap(0x20, 0x28);
	asm volatile ("sti");

	asm volatile ("int $0xD");

	while(true){
		//move_entry(0,0);
	}
}
