// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials

#include "vesavga.h"
#include "common.h"
#include "multiboot2.h"
#include "timer.h"
#include "paging.h"
#include "isr.h"
	
extern void beep( u16int freq );
extern void vga43(void);

#include "descriptor_tables.h"


void helloWorld();
volatile int foobar = 0;

extern "C" 
{
	
	extern u32int placement_address;

	void page_fault(registers_t regs);
	void init_gdt_table();
	void init_idt_table();
	void initialise_paging32(u32int maxMem);
	void initPaging32(u32int maxMem);
	

	static void init_monitor()
	{
		set_foreground_color( BLUE );
		set_background_color( LTGRAY );
		monitor_clear();
	}
	
	static void init_GDT()
	{
		monitor_write("Initialize GDT\n");
		init_gdt_table();
	}
	
	static void init_IDT()
	{
		monitor_write("Initialize IDT\n");
		init_idt_table();
	}
		
	static void init_page()
	{
		monitor_write("Initialize Paging....\n");
		monitor_write("placement_address : 0x");
		monitor_write_hex(placement_address);
		initialise_paging();
		monitor_write("\nAfter Initialize Paging....\n");
		monitor_write("placement_address : 0x");
		monitor_write_hex(placement_address);	
		monitor_write("\n");
	}

		
	static void init_page32()
	{
		monitor_write("Initialize Paging 32....\n");
		// initialise_paging32(0x1000000);
		initPaging32(0x1000000);
	}
	
	
	static void handle_page_faults()
	{
		monitor_write("Register Page Fault Handler\n");
		register_interrupt_handler(14, page_fault);
	}
	
	static void test_page_fault()
	{
		monitor_write("Testing Page Fault\n");
		u32int *ptr = (u32int*)0xA0000000;
		u32int do_page_fault = *ptr;
		monitor_write_dec(do_page_fault);
		monitor_write("After page fault! SHOULDN'T GET HERE \n");
	}
	
	int kmain32(unsigned long magic, multiboot_tag *mboot_ptr)
	{	
		
		init_monitor();
		init_GDT();
		init_IDT();
		handle_page_faults();
		init_page32();
		
		monitor_write("Hello, paging world!\n");	
		test_page_fault();
		return 0;
	}

}
