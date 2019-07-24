#include <stdio.h>
#include "common.h"
#include "vesavga.h"
#include "serial.h"
#include "ata.h"
#include "multiboot2.h"

void init_idt64_table();

uint32_t mboot_header=0;

volatile int foo___ = 0;
static void test_page_fault();

void cmain (unsigned long magic, unsigned long addr);


extern "C"
{
void kmain64()
{

	char buffer[1024];
	sprintf(buffer, "1\n");
	monitor_write(buffer);
	
	SYM6432(set_foreground_color)(GREEN);
	SYM6432(set_background_color)(BLACK);
	monitor_clear();
	sprintf(buffer, "%s", "Hello World from 64-bit long mode!!!!!\n");
	monitor_write(buffer);
	sprintf(buffer, "%s", "Init the 64-bit interrupt table\n");
	monitor_write(buffer);
	init_idt64_table();
	sprintf(buffer, "%s", "64-bit interrupt table is initailzed!!!\n");
	monitor_write(buffer);
	
	auto success = init_serial(1, BAUD_38400, BITS_8, PARITY_NONE, NO_STOP_BITS);

	if( success == SUCCESS)
	{
	 	sprintf(buffer, "%s", "Initialized COM1 port\n");
		monitor_write(buffer);
	}

	detectControllers();

	if( mboot_header != 0)
	{
		sprintf(buffer, "%s", "Dump mboot_header\n");
		monitor_write(buffer);
		cmain(MULTIBOOT2_BOOTLOADER_MAGIC, mboot_header);
	}
	else
	{
		sprintf(buffer, "%s", "mboot_header is NULL\n");
		monitor_write(buffer);
	}
	
	
	test_page_fault();
	
	// int *badPtr = reinterpret_cast<int *>(0x00F0F0F0F0F0F0F0u);
	
	// foo___ = *(badPtr);
	
	// monitor_write_dec(foo___);
	
}

}

	
static void test_page_fault()
{
	monitor_write("Testing Page Fault\n");
	uint32_t *ptr = (uint32_t*)0xA0000000;
	uint32_t do_page_fault = *ptr;
	monitor_write_dec(do_page_fault);
	monitor_write("After page fault! SHOULDN'T GET HERE \n");
}
