#include "kmalloc.h"
#include "vesavga.h"

extern u32int end;

u32int placement_address = (u32int)(&end);

static u32int kmalloc_generic(u32int sz, int align, u32int *phys);


u64int kmalloc64(u32int sz)
{
	return (u64int)kmalloc(sz);
}

u64int kmalloc64_aligned(u32int sz)
{
	return (u64int)kmalloc_aligned(sz);
}

u64int kmalloc64_phys(u32int sz, u64int *phys)
{
	if( phys )
	{
		u32int p=0;
		u32int tmp = kmalloc_phys(sz, &p);
		*phys = p;
		return tmp;
	}
	else
	{
		return (u64int)kmalloc_phys(sz, NULL);
	}
}

u64int kmalloc64_aligned_phys(u32int sz, u64int *phys)
{
	if( phys )
	{
		u32int p=0;
		u32int tmp = kmalloc_aligned_phys(sz, &p);
		*phys = p;
		return tmp;
	}
	else
	{
		return (u64int)kmalloc_aligned_phys(sz, NULL);
	}
}

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
	u32int retVal =  kmalloc_generic(sz,0,phys);
	if(phys)
	{
		printf32("kmalloc_phys: ptr: 0x%08.8x, phys: 0x%08.8x\n", retVal, (*phys));
	}
	return retVal;
}

u32int kmalloc_aligned_phys(u32int sz, u32int *phys)
{
	u32int retVal = kmalloc_generic(sz, 1, phys);
	if(phys)
	{
		printf32("kmalloc_aligned_phys: ptr: 0x%08.8x, phys: 0x%08.8x\n", retVal, (*phys));
	}
	return retVal;
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
	// printf("Placement: 0x%08.8x\n", tmp);
	return tmp;
}
