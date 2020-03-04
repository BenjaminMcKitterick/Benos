
# disable interrupts at the start of each macro then jump to
# isr handler after executing appropriate instructions.

.macro isr_dummycode arg
	.global isr\arg
	.align 16
	isr\arg:
		cli
		push  $0						# push the dummy error code
		push  $\arg					# push the interrupt num
		jmp isr_caller_dummy
.endm

.macro isr_errorcode arg
	.global isr\arg
	.align 16
	isr\arg:
		cli
		push $\arg						# push the interrupt num
		jmp isr_caller_error
.endm

.macro irq irqn, isrn
	.global irq\irqn
	.align 16
	irq\irqn:
		cli
		push $0							# push irq number
		push $\isrn					# push remapped isr number
		jmp irq_caller
.endm

isr_dummycode 0
isr_dummycode 1
isr_dummycode 2
isr_dummycode 3
isr_dummycode 4
isr_dummycode 5
isr_dummycode 6
isr_dummycode 7
isr_errorcode 8
isr_dummycode 9
isr_errorcode 10
isr_errorcode 11
isr_errorcode 12
isr_errorcode 13
isr_errorcode 14
isr_dummycode 15
isr_dummycode 16
isr_dummycode 17
isr_dummycode 18
isr_dummycode 19
isr_dummycode 20
isr_dummycode 21
isr_dummycode 22
isr_dummycode 23
isr_dummycode 24
isr_dummycode 25
isr_dummycode 26
isr_dummycode 27
isr_dummycode 28
isr_dummycode 29
isr_dummycode 30
isr_dummycode 31
irq 0,  32
irq 1,  33
irq 2,  34
irq 3,  35
irq 4,  36
irq 5,  37
irq 6,  38
irq 7,  39
irq 8,  40
irq 9,  41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

.extern isr_handler

isr_caller_dummy:					# pushes all general purpose registers

	pusha
	push %eax

	call isr_handler

	pop %eax
	popa

	add $8, %esp			# 8|4 clear error codes

	sti								# enable interrupts
	iret

isr_caller_error:					# pushes all general purpose registers

	pusha
	push %eax

	call isr_handler

	pop %eax
	popa

	add $4, %esp			# 8|4 clear error codes

	sti								# enable interrupts
	iret

.extern irq_handler

irq_caller:

	pusha
	push %eax

	call irq_handler

	pop %eax
	popa

	add $8, %esp			# 8|4 clear error codes

	sti
	iret
