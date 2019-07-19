#include "common.h"
#include "descriptor_tables.h"
#include "isr.h"
#include "memcpy.h"

// Lets us access our ASM functions from our C code.

extern "C" {
extern void idt_flush(uintptr_t);
}

static void init_idt();
static void init_irqs();
static void idt_set_gate(uint8_t,uintptr_t,uint16_t,uint8_t);

idt_entry64_t idt_entries[256] = { 0 };
idt_ptr64_t   idt_ptr{ 0 };


// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
extern "C"
{	
	void init_idt_table()
	{
		init_idt();
		memset(&interrupt_handlers, 0, sizeof(isr_t)*256);		
	}
}

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */


static void remap_pics()
{
	// Remap the irq table.
	outb(MASTER_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(SLAVE_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

#undef outb

static void init_irqs()
{

	idt_set_gate(32, (uintptr_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uintptr_t)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uintptr_t)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uintptr_t)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uintptr_t)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uintptr_t)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uintptr_t)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uintptr_t)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uintptr_t)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uintptr_t)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uintptr_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uintptr_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uintptr_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uintptr_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uintptr_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uintptr_t)irq15, 0x08, 0x8E);
}


static void init_isrs()
{
	idt_set_gate( 0, (uintptr_t)isr0 , 0x08, 0x8E);
	idt_set_gate( 1, (uintptr_t)isr1 , 0x08, 0x8E);	
	idt_set_gate( 2, (uintptr_t)isr2 , 0x08, 0x8E);
	idt_set_gate( 3, (uintptr_t)isr3 , 0x08, 0x8E);
	idt_set_gate( 4, (uintptr_t)isr4 , 0x08, 0x8E);
	idt_set_gate( 5, (uintptr_t)isr5 , 0x08, 0x8E);
	idt_set_gate( 6, (uintptr_t)isr6 , 0x08, 0x8E);
	idt_set_gate( 7, (uintptr_t)isr7 , 0x08, 0x8E);
	idt_set_gate( 8, (uintptr_t)isr8 , 0x08, 0x8E);
	idt_set_gate( 9, (uintptr_t)isr9 , 0x08, 0x8E);
	idt_set_gate( 10, (uintptr_t)isr10 , 0x08, 0x8E);
	idt_set_gate( 11, (uintptr_t)isr11 , 0x08, 0x8E);
	idt_set_gate( 12, (uintptr_t)isr12 , 0x08, 0x8E);
	idt_set_gate( 13, (uintptr_t)isr13 , 0x08, 0x8E);
	idt_set_gate( 14, (uintptr_t)isr14 , 0x08, 0x8E);
	idt_set_gate( 15, (uintptr_t)isr15 , 0x08, 0x8E);
	idt_set_gate( 16, (uintptr_t)isr16 , 0x08, 0x8E);
	idt_set_gate( 17, (uintptr_t)isr17 , 0x08, 0x8E);
	idt_set_gate( 18, (uintptr_t)isr18 , 0x08, 0x8E);
	idt_set_gate( 19, (uintptr_t)isr19 , 0x08, 0x8E);
	idt_set_gate( 20, (uintptr_t)isr20 , 0x08, 0x8E);
	idt_set_gate( 21, (uintptr_t)isr21 , 0x08, 0x8E);
	idt_set_gate( 22, (uintptr_t)isr22 , 0x08, 0x8E);
	idt_set_gate( 23, (uintptr_t)isr23 , 0x08, 0x8E);
	idt_set_gate( 24, (uintptr_t)isr24 , 0x08, 0x8E);
	idt_set_gate( 25, (uintptr_t)isr25 , 0x08, 0x8E);
	idt_set_gate( 26, (uintptr_t)isr26 , 0x08, 0x8E);
	idt_set_gate( 27, (uintptr_t)isr27 , 0x08, 0x8E);
	idt_set_gate( 28, (uintptr_t)isr28 , 0x08, 0x8E);
	idt_set_gate( 29, (uintptr_t)isr29 , 0x08, 0x8E);
	idt_set_gate( 30, (uintptr_t)isr30 , 0x08, 0x8E);
	idt_set_gate( 31, (uintptr_t)isr31 , 0x08, 0x8E);

}

static void init_idt()
{
	idt_ptr.limit() = sizeof(idt_entry_t) * 256 -1;
	idt_ptr.base()  = (uintptr_t)&idt_entries;


	memset(&idt_entries, 0, sizeof(idt_entry_t)*256);
	
	// Remap the irq table.
	remap_pics();
	
	init_isrs();
	init_irqs();

	idt_flush((uintptr_t)&idt_ptr);
}

static void idt_set_gate(uint8_t num, uintptr_t base, uint16_t sel, uint8_t flags)
{
	idt_entries[num].set(base, sel, flags);
#if 0
	idt_entries[num].base_lo() = base & 0xFFFF;
	idt_entries[num].base_hi() = (base >> 16) & 0xFFFF;

	idt_entries[num].sel()     = sel;
	idt_entries[num].always0() = 0;
	// We must uncomment the OR below when we get to using user-mode.
	// It sets the interrupt gate's privilege level to 3.
	idt_entries[num].flags()   = flags /* | 0x60 */;
#endif // 0
}