#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED
//
// isr.h -- Interface and structures for high level interrupt service routines.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"

typedef struct registers
{
   u32int ds;                  // Data segment selector
   u32int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   u32int int_no, err_code;    // Interrupt number and error code (if applicable)
   u32int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;

// A few defines to make life a little easier
// for the remapped IRQ's
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define MASTER_PIC_COMMAND	PIC1
#define MASTER_PIC_DATA	(PIC1+1)
#define SLAVE_PIC_COMMAND	PIC2
#define SLAVE_PIC_DATA	(PIC2+1)

#define PIC_RESET_COMMAND 0x20

#define PIC_EOI		0x20		/* End-of-interrupt command code */

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(registers_t);
void register_interrupt_handler(u8int n, isr_t handler);

extern isr_t interrupt_handlers[256] ;

#endif // ISR_H_INCLUDED
