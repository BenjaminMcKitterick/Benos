/* Linker file consists of an entry point and sections with size
   defined. Required to link all the object files together to create
   a kernel image. */

ENTRY(load)

SECTIONS
{
	/* Put sections at 1 MiB. */
	. = 1M;

	/* code section. */
	.text 0x100000 : ALIGN(4K)
	{
    text = .; _text = .; __text = .;
		*(.multiboot)
		*(.text)
	}

	/* Read-write data section. */
	.data BLOCK(4K) : ALIGN(4K)
	{
    data = .; _data = .; __data = .;
		*(.data)
	}

	/* Read-write data and stack bss section */
	.bss BLOCK(4K) : ALIGN(4K)
	{
    bss = .; _bss = .; __bss = .;
		*(COMMON)
		*(.bss)
	}

  end = .; _end = .; __end = .;
}
