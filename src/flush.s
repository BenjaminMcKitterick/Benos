.global load_gdt
.extern gdt_pointer

load_gdt:
	 lgdt (gdt_pointer)		  # load the new gdt pointer

	 mov $0x10, %ax					# 0x10 is offset to data segment
	 mov %ds, %ax						# data segment register
	 mov %es, %ax						# extra segment register
	 mov %fs, %ax						# file segment register
	 mov %gs, %ax						# general segment register
	 mov %ss, %ax						# stack segment register
	 jmp $0x08, $load		  	# 0x08 offset to get to code segment

load:
   ret

.global load_idt
.extern idt_pointer

load_idt:
	lidt (idt_pointer)			# load the new idt pointer
	ret

# need to set global tings and allow the external var to be parsed.
# using 0x80000001 will set PG and PE bits in CR0 reg

.global enable_paging
.extern dir_address
enable_paging:
		mov $dir_address, %eax
		mov %eax, %cr3
		mov %cr0, %eax
		or $0x80000001, %eax
		mov %eax, %cr0
		ret
