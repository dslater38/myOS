#include "kheap.h"

	
heap_t::heap_t(uint32_t start, uint32_t end_addr, uint32_t max_size, uint8_t super, uint8_t ro)
	: index{place_ordered_array<header_t>((void*)start, HEAP_INDEX_SIZE)}
	, start_address{start}
	, end_address{end_addr}
	, max_address{max_size}
	, supervisor{super}
	, readonly{ro}
{
	// All our assumptions are made on startAddress and endAddress being page-aligned.
	ASSERT(start%0x1000 == 0);
	ASSERT(end_addr%0x1000 == 0);

	// Shift the start address forward to resemble where we can start putting data.
	start_address += sizeof(type_t)*HEAP_INDEX_SIZE;
	PAGE_ALIGN(start_address);
		
	// We start off with one large hole in the index.
	header_t *hole = (header_t *)start_address;
	hole->size = end_addr-start_address;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array((void*)hole, &heap->index);
}

// static 
heap_t *heap_t::create(uint32_t start, uint32_t end_addr, uint32_t max_size, uint8_t supervisor, uint8_t readonly)
{
	return New<heap_t>(start, end_addr, max_size, supervisor, readonly);
}

int64_t heap_t::find_smallest_hole(uint64_t size, bool page_align)
{
	// Find the smallest hole that will fit.
	uint64_t iterator = 0;
	while (iterator < index.size)
	{
		header_t *header = (header_t *)lookup_ordered_array(iterator, &index);
		// If the user has requested the memory be page-aligned
		if (page_align > 0)
		{
			// Page-align the starting point of this header.
			uint64_t location = (uint64_t )header;
			int64_t offset = 0;
			if ((location+sizeof(header_t)) & 0xFFFFFFFFFFFFF000ull != 0)
			{
				offset = 0x1000 /* page size */  - (location+sizeof(header_t))%0x1000;
			}
			int64_t hole_size = (int64_t)header->size - offset;
			// Can we fit now?
			if (hole_size >= (int64_t)size)
				break;
		}
		else if (header->size >= size)
			break;
		iterator++;
	}
	// Why did the loop exit?
	if (iterator == heap->index.size)
	{
		return -1; // We got to the end and didn't find anything.
	}
	else
	{
		return iterator;
	}
	}
}
