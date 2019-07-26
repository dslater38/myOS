// main.c -- Defines the C-code kernel entry point, calls initialisation routines.
// Made for JamesM's tutorials

#include "common.h"
#include "vesavga.h"
#include "multiboot2.h"
#include "timer.h"
#include "isr.h"
#include "descriptor_tables.h"
#include "elf/elf.h"

constexpr uint64_t	B=1;
constexpr uint64_t	KB=1024*B;
constexpr uint64_t	MB=1024*KB;
constexpr uint64_t	GB=1024*MB;
constexpr uint64_t	TB=1024*GB;


void cmain (unsigned long magic, unsigned long addr);
extern void *x64entry;

extern "C" 
{
	
	// static void *load_elf64_module(Elf64_Header *elfFile);
	extern uint32_t end;
	uint64_t placement_address = (uint64_t)(&end);
	uint32_t mboot_header=0;
	
	extern void gdt_flush64(uint32_t);
	
	extern uint64_t placement_address;
	void init_gdt_table64(uint32_t entryPoint);
	void init_idt_table();
	void initPaging64(uint64_t maxMem);

	int kmain32(unsigned long magic, multiboot_tag *mboot_ptr)
	{	
		printf("=============================================================\n");
		printf("mboot_ptr: 0x%08.8x, size: %d, end 0x%08.8x\n", reinterpret_cast<uint32_t>(mboot_ptr), mboot_ptr->type, reinterpret_cast<uint32_t>(mboot_ptr) + mboot_ptr->type);
		printf("placement_address: 0x%08.8x\n", (uint32_t)placement_address);
		// placement_address = (uint64_t)&end;
		// printf("placement_address: 0x%08.8x\n", (uint32_t)placement_address);
		cmain(magic, (uint32_t)mboot_ptr);
		initPaging64(8*MB);
	
		init_gdt_table64(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(x64entry)));
		

		return 0;
	}
}
