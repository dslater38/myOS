// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials

#include "vesavga.h"
#include "common.h"
#include "multiboot2.h"
#include "timer.h"
#include "isr.h"
#include "descriptor_tables.h"

extern "C" 
{
	
	extern void gdt_flush64(uint32_t);
	extern uint32_t end;
	extern uint64_t placement_address;
	static void init_gdt_table64();
	void init_idt_table();
	void initPaging64(uint64_t maxMem);

	int kmain32(unsigned long magic, multiboot_tag *mboot_ptr)
	{	

		printf32("placement_address: 0x%08.8x\n", (uint32_t)placement_address);
		// placement_address = (uint64_t)&end;
		// printf32("placement_address: 0x%08.8x\n", (uint32_t)placement_address);
		
		initPaging64(0x10000000);

		init_gdt_table64();

		return 0;
	}
}

// 64-bit GDT entries
static gdt_entry_t gdt_entries[7] = { 0 };
static gdt64_ptr_t   gdt64_ptr{ 0 };


static void init_gdt_table64()
{
	gdt64_ptr.limit() = sizeof(gdt_entries) - 1;
	gdt64_ptr.base()  = (uint64_t)&gdt_entries; 

	gdt_entries[0].set(0, 0, 0, 0, SYSTEM_DESCRIPTOR);
	gdt_entries[1].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|IS_64BITS|CODE_SEGMENT_CONFORMING_BIT), RING0, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[2].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED|IS_64BITS), RING0, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[3].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|IS_64BITS|CODE_SEGMENT_CONFORMING_BIT), RING3, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[4].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED|IS_64BITS), RING3, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[5].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|CODE_SEGMENT_CONFORMING_BIT), RING3, (SEGMENT_PRESENT|GRAN_1K|OPERAND_SIZE_32));
	gdt_entries[6].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED), RING3, (SEGMENT_PRESENT|GRAN_1K|OPERAND_SIZE_32));


 	gdt_flush64((uint32_t)&gdt64_ptr);
}

#include "sym6432.h"
#include "kmalloc.6432.cpp"
#include "memset.6432.cpp"
#include "panic.6432.cpp"
#include "strlen.6432.cpp"
#include "vesavga.6432.cpp"
#include "vsprintf.6432.cpp"
#include "page64_tables.32.cpp"

