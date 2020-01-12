/* Linker file consists of an entry point and sections with size
   defined. Required to link all the object files together to create
   a kernel image. */

ENTRY(load)
 
SECTIONS
{
	/* Put sections at 1 MiB. */
	. = 1M;
 
	/* text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
 
	/* Read-only data section. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 
	/* Read-write data section. */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	/* Read-write data and stack bss section */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}
