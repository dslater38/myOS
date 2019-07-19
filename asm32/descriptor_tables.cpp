#include "common.h"
#include "descriptor_tables.h"
#include "isr.h"
#include "memcpy.h"

// Lets us access our ASM functions from our C code.

extern "C" {
extern void gdt_flush(uint32_t);
extern void gdt_flush64(uint32_t);
extern void idt_flush(uint32_t);
}

// Internal function prototypes.
static void init_gdt();
static void init_gdt64();
static void gdt_set_gate(int32_t,uint32_t,uint32_t,uint8_t,uint8_t);
static void init_idt();
static void init_irqs();
static void idt_set_gate(uint8_t,uint32_t,uint16_t,uint8_t);

gdt_entry_t gdt_entries[5] = { 0 };

gdt_entry_t gdt_entries32[5] = { 0 };

gdt_ptr_t   gdt_ptr{ 0 };

gdt64_ptr_t   gdt64_ptr{ 0 };

//~ idt_entry_t idt_entries[256] = { 0 };

//~ idt_ptr_t   idt_ptr{ 0 };


// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
extern "C"
{
	void init_gdt_table()
	{
		init_gdt();
	}
	
	void init_gdt_table64()
	{
		init_gdt64();
	}
	
	
	//~ void init_idt_table()
	//~ {
		//~ init_idt();
		//~ memset(&interrupt_handlers, 0, sizeof(isr_t)*256);		
	//~ }
	
	//~ void init_descriptor_tables()
	//~ {
		//~ // Initialise the global descriptor table.
		//~ init_gdt();
		//~ init_idt();
		//~ memset(&interrupt_handlers, 0, sizeof(isr_t)*256);
	//~ }
}

static void init_gdt()
{
 	gdt_ptr.limit() = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base()  = (uint32_t)&gdt_entries32; 

	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	gdt_flush((uint32_t)&gdt_ptr);
}


static void init_gdt64()
{
	gdt64_ptr.limit() = (sizeof(gdt_entry_t) * 5) - 1;
	gdt64_ptr.base()  = (uint64_t)&gdt_entries; 

	gdt_entries[0].set(0, 0, 0, 0, SYSTEM_DESCRIPTOR);
	gdt_entries[1].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|IS_64BITS|CODE_SEGMENT_CONFORMING_BIT), RING0, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[2].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED|IS_64BITS), RING0, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[3].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|IS_64BITS|CODE_SEGMENT_CONFORMING_BIT), RING3, (SEGMENT_PRESENT|GRAN_1K));
	gdt_entries[4].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED|IS_64BITS), RING3, (SEGMENT_PRESENT|GRAN_1K));

 	gdt_flush64((uint32_t)&gdt64_ptr);
}

// Set the value of one GDT entry.
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries32[num].set(base, limit, access, gran);
}

