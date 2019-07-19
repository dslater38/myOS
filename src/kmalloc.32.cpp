#include "kmalloc.h"
#include "vesavga.h"

// extern uint32_t end;

uint32_t placement_address = 0; // (uint32_t)end;

static uint32_t kmalloc_generic(uint32_t sz, int align, uint32_t *phys);

extern "C"
{

uint64_t kmalloc64(uint32_t sz)
{
	return (uint64_t)kmalloc(sz);
}

uint64_t kmalloc64_aligned(uint32_t sz)
{
	return (uint64_t)kmalloc_aligned(sz);
}

uint64_t kmalloc64_phys(uint32_t sz, uint64_t *phys)
{
	if( phys )
	{
		uint32_t p=0;
		uint32_t tmp = kmalloc_phys(sz, &p);
		*phys = p;
		return tmp;
	}
	else
	{
		return (uint64_t)kmalloc_phys(sz, NULL);
	}
}

uint64_t kmalloc64_aligned_phys(uint32_t sz, uint64_t *phys)
{
	if( phys )
	{
		uint32_t p=0;
		uint32_t tmp = kmalloc_aligned_phys(sz, &p);
		*phys = p;
		return tmp;
	}
	else
	{
		return (uint64_t)kmalloc_aligned_phys(sz, NULL);
	}
}

uint32_t kmalloc(uint32_t sz)
{
	return kmalloc_generic(sz,0,NULL);
}

uint32_t kmalloc_aligned(uint32_t sz)
{
	return kmalloc_generic(sz,1,NULL);
}

uint32_t kmalloc_phys(uint32_t sz, uint32_t *phys)
{
	uint32_t retVal =  kmalloc_generic(sz,0,phys);
	//~ if(phys)
	//~ {
		//~ printf32("kmalloc_phys: ptr: 0x%08.8x, phys: 0x%08.8x\n", retVal, (*phys));
	//~ }
	return retVal;
}

uint32_t kmalloc_aligned_phys(uint32_t sz, uint32_t *phys)
{
	uint32_t retVal = kmalloc_generic(sz, 1, phys);
	if(phys)
	{
	// 	printf32("kmalloc_aligned_phys: ptr: 0x%08.8x, phys: 0x%08.8x, size: %d\n", retVal, (*phys), sz);
	}
	return retVal;
}

static void page_align_placement()
{
	if( 0 != (placement_address & 0x00000FFF) )
	{
		// Align it.
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;		
	}
}

static uint32_t kmalloc_generic(uint32_t sz, int align, uint32_t *phys)
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
	uint32_t tmp = placement_address;
	placement_address += sz;
	// printf("Placement: 0x%08.8x\n", tmp);
	return tmp;
}
}
