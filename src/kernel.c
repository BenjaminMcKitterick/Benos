
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <utility.h>
#include <display.h>
#include <paging.h>
#include <heap.h>
#include <keyboard.h>
#include <pic.h>
#include <pit.h>
#include <interrupts.h>
#include <descriptors.h>

extern uint8_t first_key;

static void start_menu()
{
	for(int i = 0; i < TERMINAL_WIDTH; i++)
			print(" ", DARK_GREY, DARK_GREY);

	for(int i = 0; i < TERMINAL_WIDTH; i++)
			print("|", GREEN, BLACK);

	move_entry(TERMINAL_WIDTH/3.25, (TERMINAL_HEIGHT/3)-2);
	print("_____________________________", GREEN, BLACK);
	move_entry(TERMINAL_WIDTH/3.25, TERMINAL_HEIGHT/3);
	print("BenOS an x86 Operating System", GREEN, BLACK);
	move_entry(TERMINAL_WIDTH/3.25, (TERMINAL_HEIGHT/3)+1);
	print("_____________________________", GREEN, BLACK);

	move_entry(TERMINAL_WIDTH/3, TERMINAL_HEIGHT/1.5);
	print("Enter any key to continue", LIGHT_GREY, BLACK);

	move_entry(0, TERMINAL_HEIGHT-2);

	for(int i = 0; i < TERMINAL_WIDTH; i++)
			print("|", GREEN, BLACK);

	for(int i = 0; i < TERMINAL_WIDTH; i++)
			print(" ", DARK_GREY, DARK_GREY);
}

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

	while(1){
			if(first_key == 1)
			{
					reset_terminal();
					break;
			}
			else
			{
				start_menu();
			}
	}

	//__asm__ volatile("int $0x08");
	do_something();

	//uint32_t a = malloc_virt(8,0);
	//uint32_t b = malloc_virt(8,0);
	//uint32_t c = malloc_virt(8,0);

	//println("a %h", a);
	//println("b %h", b);
	//println("c %h", c);

	//deallocate(c);
	//deallocate(b);
	//uint32_t d = malloc_virt(12,0);
	//println("d %h", d);

	while(true){
		//move_entry(0,0);
	}
}
