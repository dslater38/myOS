#include "kmalloc.h"
#include "vesavga.h"
#include "sym6432.h"

// extern uint32_t end;

// uint32_t placement_address = 0; // (uint32_t)end;

static uintptr_t kmalloc_generic(size_t sz, int align, uintptr_t *phys);

extern "C"
{
	extern uintptr_t placement_address;

	uintptr_t SYM6432(kmalloc)(size_t sz)
	{
		return kmalloc_generic(sz,0,NULL);
	}

	uintptr_t SYM6432(kmalloc_aligned)(size_t sz)
	{
		return kmalloc_generic(sz,1,NULL);
	}

	uintptr_t SYM6432(kmalloc_phys)(size_t sz, uintptr_t *phys)
	{
		auto retVal =  kmalloc_generic(sz,0,phys);
		return retVal;
	}

	uintptr_t SYM6432(kmalloc_aligned_phys)(size_t sz, uintptr_t *phys)
	{
		auto retVal = kmalloc_generic(sz, 1, phys);
		//~ if(phys)
		//~ {
		//~ // 	printf32("kmalloc_aligned_phys: ptr: 0x%08.8x, phys: 0x%08.8x, size: %d\n", retVal, (*phys), sz);
		//~ }
		return retVal;
	}

	constexpr uintptr_t ALIGN_MASK = (static_cast<uintptr_t>(-1) << 12 );
	constexpr uintptr_t BITS_MASK = ~(ALIGN_MASK);
	
	static void page_align_placement()
	{
		if( 0 != (placement_address & BITS_MASK) )
		{
			// Align it.
			placement_address &= ALIGN_MASK;
			placement_address += 0x1000;		
		}
	}

	static uintptr_t kmalloc_generic(size_t sz, int align, uintptr_t *phys)
	{
		if( align )
		{
			page_align_placement();
		}
		//~ if (align == 1 && (placement_address & 0xFFFFF000)) // If the address is not already page-aligned
		//~ {
			//~ // Align it.
			//~ placement_address &= 0xFFFFF000;
			//~ placement_address += 0x1000;
		//~ }
		if (phys)
		{
			*phys = placement_address;
		}
		uintptr_t tmp = placement_address;
		placement_address += sz;
		// printf("Placement: 0x%08.8x\n", tmp);
		return tmp;
	}
}
