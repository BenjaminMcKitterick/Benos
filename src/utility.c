
#include <stddef.h>
#include "utility.h"
#include "display.h"

/*
	 asm is volatile so that it executes precisely where it is placed.
 	 no changes for optimisation.
*/
void port_out(uint16_t port, uint8_t byte)
{
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (byte));
}

uint8_t port_in(uint16_t port)
{
	uint8_t val;
	asm volatile ("inb %1, %0" : "=a" (val) : "dN" (port));
	return val;
}

/*
	 Sets a specific area of memory; specified by the address pointer
 	 and a size, then fills the area with whatever data has been
	 specified.
*/
void *set_memory(void *address, char data, size_t num_bytes)
{
	unsigned char* c = address;
	for( int i = 0; (size_t) i < num_bytes; i++){
		*c++ = (unsigned)data;
	}
	return address;
}
/* TODO: finish the memory allocation function. */
//void *allocate_memory(uint32_t size, uint8_t align)
//{
		//void *memory_pointer;
//}

/* TODO: finish the free memory function. */
//void free_memory(void *memory_pointer)
//{

//}

/*
	 Simple funtion that returns the length of a string by counting
	 the amount of characters.
*/
int str_len(char* string)
{
	int i = 0;
	while(string[i] != '\0'){
		i++;
	}
	return i;
}
