#ifndef KHEAP_H_INCLUDED
#define KHEAP_H_INCLUDED

#include "common.h"
#include "ordered_array.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000
#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

struct header_t
{
	uint32_t magic{HEAP_MAGIC};
	uint32_t size{0};
	uint8_t is_hole{1};
	header_t(uint32_t size_, uint8_t hole)
		: magic{HEAP_MAGIC}
		, size{size_}
		, is_hole{hole}
		{
		}
};

struct footer_t
{
	uint32_t magic{HEAP_MAGIC};
	header_t	*header{0};
	explicit footer_t(header_t *h)
		: magic{HEAP_MAGIC}
		, header{h}
		{
		}
};

template<>
struct less<header_t>
{
	uint8_t operator()(const header_t &a, const header_t& b)
	{
		return a.size()<b.size() ? 1 : 0;
	}
};

struct heap_t
{
	ordered_array_t<header_t> index{};
	uint32_t start_address{0}; // The start of our allocated space.
	uint32_t end_address{0};   // The end of our allocated space. May be expanded up to max_address.
	uint32_t max_address{0};   // The maximum address the heap can be expanded to.
	uint8_t supervisor{0};     // Should extra pages requested by us be mapped as supervisor-only?
	uint8_t readonly{0};       // Should extra pages requested by us be mapped as read-only?
	
	heap_t(uint32_t start, uint32_t end_addr, uint32_t max_size, uint8_t super, uint8_t ro)
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
	
	static heap_t *create(uint32_t start, uint32_t end_addr, uint32_t max_size, uint8_t supervisor, uint8_t readonly)
	{
		return new(kmalloc(sizeof(heap_t))) heap_t{start, end_addr, max_size, supervisor, readonly};
	}
};

#endif // KHEAP_H_INCLUDED
