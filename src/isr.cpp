//
// isr.c -- High level interrupt service routines and interrupt request handlers.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"
#include "isr.h"
#include "vesavga.h"

isr64_t interrupt64_handlers[256] = {0};

/* Helper func */
static uint16_t __pic_get_irq_reg(uint8_t ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(MASTER_PIC_COMMAND, ocw3);
    outb(SLAVE_PIC_COMMAND, ocw3);
    return (static_cast<uint16_t>(inb(SLAVE_PIC_COMMAND) << 8) | inb(MASTER_PIC_COMMAND));
}
 
/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}
 
/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}

extern "C"
{
	// This gets called from our ASM interrupt handler stub.
	void isr64_handler(registers64_t regs)
	{
		printf("interrupt: %ld", regs.int_no);
		if (interrupt64_handlers[regs.int_no] != 0)
		{
		//	printf("Call ISR handler %ld\n", regs.int_no);
			isr64_t handler = interrupt64_handlers[regs.int_no];
			handler(regs);
		//	printf("ISR handler %ld returned\n", regs.int_no);
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

	static inline void sendPICEoi(uint8_t irq)
	{
		if( irq >= IRQ8 )
		{
			// Send reset signal to slave.
			outb(SLAVE_PIC_COMMAND, PIC_RESET_COMMAND);
		}
		outb(MASTER_PIC_COMMAND, PIC_RESET_COMMAND);
	}

	// This gets called from our ASM interrupt handler stub.
	void irq64_handler(registers64_t regs)
	{
		auto call_handler = true;
		auto spurious = false;
		const auto irq = regs.int_no;
		if(IRQ7 == irq)
		{
			auto isr = pic_get_isr();
			if( (isr & 0x80) == 0 )
			{
				spurious = true;
				call_handler = false;
			}
		}
		// Send an EOI (end of interrupt) signal to the PICs.
		// If this interrupt involved the slave.
		if (irq >= IRQ8)
		{
			if(IRQ15 == irq)
			{
				auto isr = pic_get_isr();
				if( (isr & 0x8000) == 0 )
				{
					spurious = true;
					call_handler = false;
				}
			}
			// if( spurious == false )	// don't send EOI for spurious IRQ15
			// {
			// 	// Send reset signal to slave.
			// 	outb(SLAVE_PIC_COMMAND, PIC_RESET_COMMAND);
			// }
		}

		// if( spurious == false )	// don't send EOI for spurious IRQ7.
		// {						// Note: We do send an EOI to the master pic for a spurious IRQ15
		// 	// Send reset signal to master. (As well as slave, if necessary).
		// 	outb(MASTER_PIC_COMMAND, PIC_RESET_COMMAND);
		// }

		if( call_handler == true )
		{
			if (interrupt64_handlers[regs.int_no] != 0)
			{
				debug_out("Call IRQ handler %ld\n", irq);
				isr64_t handler = interrupt64_handlers[irq];
				handler(regs);
			//	printf("IRQ handler %ld returned\n", regs.int_no);
			}
			else
			{
				// PANIC1("no interrupt handler for: %ld", regs.int_no);
				debug_out("no interrupt handler for: %ld\n", irq);
			}
		}
		else
		{
			debug_out("Spurious IRQ %ld\n", irq);
		}

		if( spurious == false )
		{
			sendPICEoi(irq);
		}
		else if(irq >= IRQ8 )
		{
			// still need to reset the master PIC if the 
			// spurious interrupt went to the salve PIC.
			sendPICEoi(IRQ0);
		}
	}
}
