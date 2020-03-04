/* Multiboot bootloader file to instruct GRUB to load Benos */

/* Set magic number so bootloader can find header
   (starting point) of the kernel being loaded.
   Set the checksum and then flags to 0. */

.set MAGIC,    	0x1BADB002
.set FLAGS,	0
.set CHECKSUM, 	-(MAGIC + FLAGS)

/* Enable multiboot and define each of the previously set
   variables to type long. */

.section .multiboot
.long MAGIC
.long FLAGS
.long CHECKSUM

/* The stack must 16-byte alligned according to System V ABI
   standard. Bottom of the stack is set then 2048 bytes are
   allocated to it. The stack grows down on x86. */

stack_bot:
.skip 2048
stack_top:

.extern text

/* Linker script specifies _start as the beginning of the kernel
   so the bootloader jumps to this position on load. */
.section .text
.global load
.type load, @function
load:

	/* set the current stack pointer to the top of the stack. */
	mov $stack_top, %esp

	/* call the main kernel */
	call kernel_main

	/* clear the interrupt enable flags then create an infinite loop*/

	cli
loop:
	hlt
	jmp loop

/* Set size of the _start symbol to the current location '.' minus
   its start. */
.size load, . - load
