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
	u32int magic{HEAP_MAGIC};
	u32int size{0};
	u8int is_hole{1};
	header_t(u32int size_, u8int hole)
		: magic{HEAP_MAGIC}
		, size{size_}
		, is_hole{hole}
		{
		}
};

struct footer_t
{
	u32int magic{HEAP_MAGIC};
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
	u8int operator()(const header_t &a, const header_t& b)
	{
		return a.size()<b.size() ? 1 : 0;
	}
};

struct heap_t
{
	ordered_array_t<header_t> index{};
	u32int start_address{0}; // The start of our allocated space.
	u32int end_address{0};   // The end of our allocated space. May be expanded up to max_address.
	u32int max_address{0};   // The maximum address the heap can be expanded to.
	u8int supervisor{0};     // Should extra pages requested by us be mapped as supervisor-only?
	u8int readonly{0};       // Should extra pages requested by us be mapped as read-only?
	
	heap_t(u32int start, u32int end_addr, u32int max_size, u8int super, u8int ro)
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
	
	static heap_t *create(u32int start, u32int end_addr, u32int max_size, u8int supervisor, u8int readonly)
	{
		return new(kmalloc(sizeof(heap_t))) heap_t{start, end_addr, max_size, supervisor, readonly};
	}
};

#endif // KHEAP_H_INCLUDED
