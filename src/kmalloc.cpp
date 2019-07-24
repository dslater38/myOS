#include "kmalloc.h"
#include "vesavga.h"
#include "sym6432.h"

extern "C"
{
	constexpr uint64_t ALIGN_MASK = 0xFFFFFFFFFFFFF000ull;
	constexpr uint64_t BITS_MASK = 0x0000000000000FFFull;
	extern uint64_t placement_address;
	static uintptr_t kmalloc_generic(size_t sz, int align, uintptr_t *phys);

	STATIC32 uintptr_t SYM6432(kmalloc)(size_t sz)
	{
		return kmalloc_generic(sz,0,NULL);
	}

	STATIC32 uintptr_t SYM6432(kmalloc_aligned)(size_t sz)
	{
		return kmalloc_generic(sz,1,NULL);
	}

	STATIC32 uintptr_t SYM6432(kmalloc_phys)(size_t sz, uintptr_t *phys)
	{
		auto retVal =  kmalloc_generic(sz,0,phys);
		return retVal;
	}

	STATIC32 uintptr_t SYM6432(kmalloc_aligned_phys)(size_t sz, uintptr_t *phys)
	{
		auto retVal = kmalloc_generic(sz, 1, phys);
		return retVal;
	}
	
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

		uintptr_t tmp = static_cast<uintptr_t>(placement_address);
		if (phys)
		{
			*phys = tmp;
		}
		
		placement_address += sz;
		// printf("Placement: 0x%08.8x\n", tmp);
		return tmp;
	}
}
