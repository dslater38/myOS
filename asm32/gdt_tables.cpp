#include "common.h"
#include "descriptor_tables.h"

// Lets us access our ASM functions from our C code.

extern "C" {
	
	extern void gdt_flush(uint32_t);

	gdt64_ptr_t   gdt64_ptr{ 0 };
	gdt_entry_t gdt_entries[7] = { 0 };


	void *init_gdt()
	{
		gdt64_ptr.limit() = sizeof(gdt_entries) - 1;
		gdt64_ptr.base()  = (uint64_t)(uint32_t)&gdt_entries; 

		gdt_entries[0].set(0, 0, 0, 0, SYSTEM_DESCRIPTOR);
		gdt_entries[1].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|IS_64BITS|CODE_SEGMENT_CONFORMING_BIT), RING0, (SEGMENT_PRESENT|GRAN_1K));
		gdt_entries[2].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED|IS_64BITS), RING0, (SEGMENT_PRESENT|GRAN_1K));
		gdt_entries[3].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|IS_64BITS|CODE_SEGMENT_CONFORMING_BIT), RING3, (SEGMENT_PRESENT|GRAN_1K));
		gdt_entries[4].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED|IS_64BITS), RING3, (SEGMENT_PRESENT|GRAN_1K));
		gdt_entries[5].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_CODE|SEGMENT_EXECUTE_ENABLED|CODE_SEGMENT_CONFORMING_BIT), RING3, (SEGMENT_PRESENT|GRAN_1K|OPERAND_SIZE_32));
		gdt_entries[6].set(0, 0xFFFFFFFF, (SEGMENT_TYPE_DATA|SEGMENT_WRITE_ENABLED), RING3, (SEGMENT_PRESENT|GRAN_1K|OPERAND_SIZE_32));


		return &gdt64_ptr;
	}
}