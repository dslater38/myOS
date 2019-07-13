#include "kmalloc.h"

extern u32int end;

u32int placement_address = (u32int)(&end);

static u32int kmalloc_generic(u32int sz, int align, u32int *phys);

u32int kmalloc(u32int sz)
{
	return kmalloc_generic(sz,0,NULL);
}

u32int kmalloc_aligned(u32int sz)
{
	return kmalloc_generic(sz,1,NULL);
}

u32int kmalloc_phys(u32int sz, u32int *phys)
{
	return kmalloc_generic(sz,0,phys);
}

u32int kmalloc_aligned_phys(u32int sz, u32int *phys)
{
	return kmalloc_generic(sz, 1, phys);
}

static void page_align_placement()
{
	if( 0 != (placement_address | 0x00000FFF) )
	{
		// Align it.
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;		
	}
}

static u32int kmalloc_generic(u32int sz, int align, u32int *phys)
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
	u32int tmp = placement_address;
	placement_address += sz;
	return tmp;
}
