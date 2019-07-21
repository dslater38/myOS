// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials

#include "vesavga.h"
#include "common.h"
#include "multiboot2.h"
#include "timer.h"
#include "isr.h"
// #include "serial.h"
	
extern void beep( uint16_t freq );
extern void vga43(void);

#include "descriptor_tables.h"


void helloWorld();
volatile int foobar = 0;

extern "C" 
{

	extern uint32_t end;
	
	extern uint64_t p4_table[512];
	extern uint64_t p3_table[512];
	extern uint64_t p2_table[512];
	extern uint64_t p1_table[512*4096];
	
	extern uint32_t end;
	extern uint32_t placement_address;

	void page_fault(registers_t regs);
	void init_gdt_table();
	void init_gdt_table64();
	void init_idt_table();
	void initPaging32(uint32_t maxMem);
	void initPaging64(uint64_t maxMem);
	void enable_paging_64(void);
	void enable_paging_64_2(void);
	
	

	static void init_monitor()
	{
		set_foreground_color32( BLUE );
		set_background_color32( LTGRAY );
		monitor_clear32();
	}
	
	static void init_GDT()
	{
		monitor_write32("Initialize GDT\n");
		// init_gdt_table();
		init_gdt_table64();
	}
	
	static void init_IDT()
	{
		monitor_write32("Initialize IDT\n");
		init_idt_table();
	}
		
	
	static void init_page32()
	{
		monitor_write32("Initialize Paging 32....\n");
		initPaging32(0x1000000);
	}
	
	
	static void init_page64()
	{
		monitor_write32("Initialize Paging 64....\n");
		initPaging64(0x10000000);
		// enable_paging_64();
		// enable_paging_64_2();
	}

	
	static void handle_page_faults()
	{
		monitor_write32("Register Page Fault Handler\n");
		register_interrupt_handler(14, page_fault);
	}
	
	static void test_page_fault()
	{
		monitor_write32("Testing Page Fault\n");
		uint32_t *ptr = (uint32_t*)0xA0000000;
		uint32_t do_page_fault = *ptr;
		monitor_write_dec32(do_page_fault);
		monitor_write32("After page fault! SHOULDN'T GET HERE \n");
	}
	
	int kmain32(unsigned long magic, multiboot_tag *mboot_ptr)
	{	
		// auto success = init_serial32(1, BAUD_38400, BITS_8, PARITY_NONE, NO_STOP_BITS);
		
		placement_address = (uint32_t)&end;
		//~ printf32("init Placement: 0x%08.8x\n", placement_address);
		//~ init_gdt_table();
		
		
//		init_serial32(1, 38400, 8, 1, 0);
		
		//~ init_monitor();
		init_page64();

		init_GDT();
//		init_IDT();
//		handle_page_faults();
		// init_page32();
		
		//~ monitor_write_hex32((uint32_t)p4_table);
		//~ monitor_write_hex32((uint32_t)p3_table);
		//~ monitor_write_hex32((uint32_t)p2_table);
		//~ monitor_write_hex32((uint32_t)p1_table);
		
		
//		monitor_write32("Hello, paging world!\n");	
//		test_page_fault();
		return 0;
	}

}
