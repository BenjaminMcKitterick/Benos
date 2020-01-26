
#include "interrupts.h"
#include "display.h"
#include "utility.h"
#include "keyboard.h"

#include <stddef.h>
#include <stdint.h>

static void double_fault(struct reg_state r);
void create_handler(struct reg_state r);
struct reg_state reg_state;
isr int_handlers[256];

const char* exceptions[] = {
  "Division By Zero",
  "Debug",
  "Non Maskable Interrupt",
  "Breakpoint",
  "Into Detected Overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No Coprocessor",
  "Double Fault",
  "Coprocessor Segement Overrun",
  "Bad TSS",
  "Segment Not Present",
  "Stack Fault",
  "General Protection Fault",
  "Page Fault",
  //"Unknown Interrupt",
  "Fail for You!",
  "Coprocessor Fault",
  "Alignment Check",
  "Machine Check"
};

void install_ISRs(){
  reg_int_handler(8, &double_fault);
}

/*
   Function to register interrupt handlers.
*/
void reg_int_handler(uint8_t n, isr handler)
{
    int_handlers[n] = handler;
}
/*
   Interrupt service routine handler. creates a handler
   for the isr and displays some information about the
   type of isr.
*/
void isr_handler(struct reg_state r)
{
  create_handler(r);

  println("");
  if ((r.int_num >= 19) & (r.int_num <= 31)){
    println("Interrupt: %h, Reserved Exception", r.int_num);
  } else if (r.int_num < 19){
    println("Interrupt: %h, %s Exception", r.int_num, exceptions[r.int_num]);
  }
  update_keyboard();
}
/*
   Interrupt request handler. Creates a handler for the irq,
   then tells the correct PIC that the interrupt has ended. If
   IRQ 8-15 (remapped 40-47) then send EOI to the slave. Always
   send an EOI to the master PIC.
*/
void irq_handler(struct reg_state r)
{
	create_handler(r);

  if(r.int_num >= 40)
		port_out(SLAVE_PIC, END_OF_INTERRUPT);

	port_out(MASTER_PIC, END_OF_INTERRUPT);
}
/*
   This function uses the typedef isr defined in the header file
   as a function pointer to a handler. Each isr is indexed via its
   interrupt number within the int_handlers[] array.
*/
void create_handler(struct reg_state r)
{
  if (int_handlers[r.int_num] != 0)
  {
    isr handler = int_handlers[r.int_num];
    handler(r);
  }
}

static void double_fault(struct reg_state r){
    set_cursor_x(0);
    println("");
    println("   Register state:");
    println("   Eflags %h %h %h %h %h %h %h %h", r.edi, r.esi,
             r.ebp, r.esp, r.ebx, r.edx, r.ecx, r.eax);
    println("   Error %h", r.error_code);
    println("   Instruction Pointer %h", r.eip);
    println("   Code Segment %h", r.cs);
    println("   CPU Flags %h", r.eflags);
    println("   user esp %h", r.useresp);
    println("   Stack Segment %h", r.ss);
    println("");
}
