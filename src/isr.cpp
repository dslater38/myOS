//
// isr.c -- High level interrupt service routines and interrupt request handlers.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"
#include "isr.h"
#include "vesavga.h"

isr64_t interrupt64_handlers[256] = {0};

extern "C"
{
	// This gets called from our ASM interrupt handler stub.
	void isr64_handler(registers64_t regs)
	{
		if (interrupt64_handlers[regs.int_no] != 0)
		{
			printf("Call ISR handler %ld\n", regs.int_no);
			isr64_t handler = interrupt64_handlers[regs.int_no];
			handler(regs);
			printf("ISR handler %ld returned\n", regs.int_no);
		}
		else
		{
			// PANIC1("Unhandled interrupt: %ld", regs.int_no);
			printf("Unhandled interrupt: %ld", regs.int_no);
		}
	}

	void register_interrupt_handler64(uint8_t n, isr64_t handler)
	{
		interrupt64_handlers[n] = handler;
	}

	// This gets called from our ASM interrupt handler stub.
	void irq64_handler(registers64_t regs)
	{
		// Send an EOI (end of interrupt) signal to the PICs.
		// If this interrupt involved the slave.
		if (regs.int_no >= 40)
		{
			// Send reset signal to slave.
			outb(SLAVE_PIC_COMMAND, PIC_RESET_COMMAND);
		}
		// Send reset signal to master. (As well as slave, if necessary).
		outb(MASTER_PIC_COMMAND, PIC_RESET_COMMAND);

		if (interrupt64_handlers[regs.int_no] != 0)
		{
			printf("Call IRQ handler %ld\n", regs.int_no);
			isr64_t handler = interrupt64_handlers[regs.int_no];
			handler(regs);
			printf("IRQ handler %ld returned\n", regs.int_no);
		}
		else
		{
			// PANIC1("no interrupt handler for: %ld", regs.int_no);
			printf("no interrupt handler for: %ld\n", regs.int_no);
		}
	}
}
