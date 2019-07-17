#include "common.h"
#include "vesavga.h"

void init_idt64_table();

volatile int foo___ = 0;
static void test_page_fault();

extern "C"
{
void kmain64()
{
	
	set_foreground_color(CYAN);
	set_background_color(RED);
	monitor_clear();
	monitor_write("Hello World from 64-bit long mode!!!!!\n");
	monitor_write("Init the 64-bit interrupt table\n");
	init_idt64_table();
	monitor_write("64-bit interrupt table is initailzed!!!\n");
	
	test_page_fault();
	
	// int *badPtr = reinterpret_cast<int *>(0x00F0F0F0F0F0F0F0u);
	
	// foo___ = *(badPtr);
	
	// monitor_write_dec(foo___);
	
}

}

	
static void test_page_fault()
{
	monitor_write("Testing Page Fault\n");
	u32int *ptr = (u32int*)0xA0000000;
	u32int do_page_fault = *ptr;
	monitor_write_dec(do_page_fault);
	monitor_write("After page fault! SHOULDN'T GET HERE \n");
}
