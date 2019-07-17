#include "common.h"
#include "descriptor_tables.h"
#include "isr.h"
#include "vesavga.h"

// Lets us access our ASM functions from our C code.
extern "C" {
extern void idt_flush64(void *);
}

extern isr64_t interrupt64_handlers[256] ;

static void idt_set_gate(u8int num, u64int base, u16int sel, u8int flags);
static void remap_pics();
static void init_irqs();
static void init_isrs();
static void init_idt();

static idt_entry64_t idt64_entries[256] = { 0 };
static idt_ptr64_t   idt64_ptr{ 0 };
void page_fault(registers64_t regs);


void init_idt64_table()
{
	init_idt();
	for( auto i =0; i<256; ++i )
	{
		interrupt64_handlers[i] = nullptr;
	}
	register_interrupt_handler64(14, page_fault);
	// memset(&interrupt64_handlers, 0, sizeof(isr64_t)*256);	
}

extern "C"
{
	void flush_idt64(void *p)
	{
		idt_flush64(p);
	}
}

static void init_idt()
{
	idt64_ptr.limit() = sizeof(idt_entry64_t) * 256 -1;
	idt64_ptr.base()  = reinterpret_cast<u64int>(&idt64_entries);


	// memset(idt64_entries, 0, sizeof(idt_entry64_t)*256);
	
	for( auto i=0u; i<256; ++i )
	{
		idt_set_gate(i, 0, 0, 0);
	}
	
	
	// Remap the irq table.
	remap_pics();
	
	init_isrs();
	init_irqs();

	flush_idt64(&idt64_ptr);
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
	outb64(MASTER_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb64(SLAVE_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb64(0x21, 0x20);
	outb64(0xA1, 0x28);
	outb64(0x21, 0x04);
	outb64(0xA1, 0x02);
	outb64(0x21, 0x01);
	outb64(0xA1, 0x01);
	outb64(0x21, 0x0);
	outb64(0xA1, 0x0);
}

static void init_irqs()
{

	idt_set_gate(32, reinterpret_cast<u64int>(irq0), 0x08, 0x8E);
	idt_set_gate(33, reinterpret_cast<u64int>(irq1), 0x08, 0x8E);
	idt_set_gate(34, reinterpret_cast<u64int>(irq2), 0x08, 0x8E);
	idt_set_gate(35, reinterpret_cast<u64int>(irq3), 0x08, 0x8E);
	idt_set_gate(36, reinterpret_cast<u64int>(irq4), 0x08, 0x8E);
	idt_set_gate(37, reinterpret_cast<u64int>(irq5), 0x08, 0x8E);
	idt_set_gate(38, reinterpret_cast<u64int>(irq6), 0x08, 0x8E);
	idt_set_gate(39, reinterpret_cast<u64int>(irq7), 0x08, 0x8E);
	idt_set_gate(40, reinterpret_cast<u64int>(irq8), 0x08, 0x8E);
	idt_set_gate(41, reinterpret_cast<u64int>(irq9), 0x08, 0x8E);
	idt_set_gate(42, reinterpret_cast<u64int>(irq10), 0x08, 0x8E);
	idt_set_gate(43, reinterpret_cast<u64int>(irq11), 0x08, 0x8E);
	idt_set_gate(44, reinterpret_cast<u64int>(irq12), 0x08, 0x8E);
	idt_set_gate(45, reinterpret_cast<u64int>(irq13), 0x08, 0x8E);
	idt_set_gate(46, reinterpret_cast<u64int>(irq14), 0x08, 0x8E);
	idt_set_gate(47, reinterpret_cast<u64int>(irq15), 0x08, 0x8E);
	

}


static void init_isrs()
{
	idt_set_gate( 0, reinterpret_cast<u64int>(isr0) , 0x08, 0x8E);
	idt_set_gate( 1, reinterpret_cast<u64int>(isr1) , 0x08, 0x8E);	
	idt_set_gate( 2, reinterpret_cast<u64int>(isr2) , 0x08, 0x8E);
	idt_set_gate( 3, reinterpret_cast<u64int>(isr3) , 0x08, 0x8E);
	idt_set_gate( 4, reinterpret_cast<u64int>(isr4) , 0x08, 0x8E);
	idt_set_gate( 5, reinterpret_cast<u64int>(isr5) , 0x08, 0x8E);
	idt_set_gate( 6, reinterpret_cast<u64int>(isr6) , 0x08, 0x8E);
	idt_set_gate( 7, reinterpret_cast<u64int>(isr7) , 0x08, 0x8E);
	idt_set_gate( 8, reinterpret_cast<u64int>(isr8) , 0x08, 0x8E);
	idt_set_gate( 9, reinterpret_cast<u64int>(isr9) , 0x08, 0x8E);
	idt_set_gate( 10, reinterpret_cast<u64int>(isr10) , 0x08, 0x8E);
	idt_set_gate( 11, reinterpret_cast<u64int>(isr11) , 0x08, 0x8E);
	idt_set_gate( 12, reinterpret_cast<u64int>(isr12) , 0x08, 0x8E);
	idt_set_gate( 13, reinterpret_cast<u64int>(isr13) , 0x08, 0x8E);
	idt_set_gate( 14, reinterpret_cast<u64int>(isr14) , 0x08, 0x8E);
	idt_set_gate( 15, reinterpret_cast<u64int>(isr15) , 0x08, 0x8E);
	idt_set_gate( 16, reinterpret_cast<u64int>(isr16) , 0x08, 0x8E);
	idt_set_gate( 17, reinterpret_cast<u64int>(isr17) , 0x08, 0x8E);
	idt_set_gate( 18, reinterpret_cast<u64int>(isr18) , 0x08, 0x8E);
	idt_set_gate( 19, reinterpret_cast<u64int>(isr19) , 0x08, 0x8E);
	idt_set_gate( 20, reinterpret_cast<u64int>(isr20) , 0x08, 0x8E);
	idt_set_gate( 21, reinterpret_cast<u64int>(isr21) , 0x08, 0x8E);
	idt_set_gate( 22, reinterpret_cast<u64int>(isr22) , 0x08, 0x8E);
	idt_set_gate( 23, reinterpret_cast<u64int>(isr23) , 0x08, 0x8E);
	idt_set_gate( 24, reinterpret_cast<u64int>(isr24) , 0x08, 0x8E);
	idt_set_gate( 25, reinterpret_cast<u64int>(isr25) , 0x08, 0x8E);
	idt_set_gate( 26, reinterpret_cast<u64int>(isr26) , 0x08, 0x8E);
	idt_set_gate( 27, reinterpret_cast<u64int>(isr27) , 0x08, 0x8E);
	idt_set_gate( 28, reinterpret_cast<u64int>(isr28) , 0x08, 0x8E);
	idt_set_gate( 29, reinterpret_cast<u64int>(isr29) , 0x08, 0x8E);
	idt_set_gate( 30, reinterpret_cast<u64int>(isr30) , 0x08, 0x8E);
	idt_set_gate( 31, reinterpret_cast<u64int>(isr31) , 0x08, 0x8E);

}

static void idt_set_gate(u8int num, u64int base, u16int sel, u8int flags)
{
	idt64_entries[num].set(base, sel, flags);
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

void page_fault(registers64_t regs)
{
	// Output an error message.
	//monitor_write("Page fault! ( ");
	char buf[32];
	// A page fault has occurred.
	// The faulting address is stored in the CR2 register.
	u64int faulting_address = get_fault_addr64();
	// u32int faulting_address;
	// asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	// The error code gives us details of what happened.
	int present   = !(regs.err_code & 0x1); // Page not present
	int rw = regs.err_code & 0x2;           // Write operation?
	int us = regs.err_code & 0x4;           // Processor was in user-mode?
	int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
	int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

	// Output an error message.
	monitor_write("Page fault! ( ");
	if (present) {monitor_write("present ");} else {monitor_write("absent ");}
	if (rw) {monitor_write("read-only ");} else {monitor_write("read/write ");}
	if (us) {monitor_write("user-mode ");} else {monitor_write("kernel-mode ");}
	if (reserved) {monitor_write("reserved ");} else {monitor_write("not-reserved ");}
	sprintf(buf, ") at 0x%08Lx\n", faulting_address);
	monitor_write(buf);
	// monitor_write(") at 0x");
	// monitor_write_hex(faulting_address);

	// monitor_write64("Page fault!!!!!!!!!!!!!!!!!!!!\n");
 	PANIC("Page fault");
}

