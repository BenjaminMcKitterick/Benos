
#include "utility.h"

#define MASTER_PIC				0x20		// base address of PIC1
#define SLAVE_PIC					0xA0		// base address of PIC2
#define MASTER_DATA				0x21
#define SLAVE_DATA				0xA1
#define END_OF_INTERRUPT	0x20		// EOI command code
#define MASK_OFF					0xFF

#define ICW1							0x10
#define CASCADE_MODE			0x01

/* Tells the pic that I no longer want to use the BIOS defaults.
   IRQs 0-7 conflict with the CPU exception which are reserved
   by Intel up to 0x1F.
*/
void pic_remap(int master_offset, int slave_offset)
{
	// begin by saving the PIC masks
	//uint8_t master_mask = port_in(MASTER_DATA);
	//uint8_t slave_mask = port_in(SLAVE_DATA);

	/* In protected mode start with initialisation sequence 0x11
	   for master and slave PIC. */

	port_out(MASTER_PIC, ICW1 | CASCADE_MODE);
	port_out(SLAVE_PIC, ICW1 | CASCADE_MODE);

	port_out(MASTER_DATA, master_offset);		// master pic offset
	port_out(SLAVE_DATA, slave_offset);			// slave pic offset
	port_out(MASTER_DATA, 4);								// slave pic at irq2
	port_out(SLAVE_DATA, 2);								// ICW3 - slave ID
	port_out(MASTER_DATA, 0x01);
	port_out(SLAVE_DATA, 0x01);
	port_out(MASTER_DATA, 0);
	port_out(SLAVE_DATA, 0);

}

/*
	 OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
   represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain
*/
uint16_t get_irq_register(int ocw3)
{
    port_out(MASTER_PIC, ocw3);
    port_out(SLAVE_PIC, ocw3);
    return port_in(SLAVE_PIC) << 8 | port_in(MASTER_PIC);
}
