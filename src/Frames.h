#ifndef PAGE_FRAMES_H_INCLUDED
#define PAGE_FRAMES_H_INCLUDED

#include <stdint.h>
#include "PageT.h"
#include "common.h"
#include "memcpy.h"

enum class PageFlags : uintptr_t
{
	PRESENT = (1<<0),
	READWRITE = (1<<1),
	USER = (1<<2),
	WRITETHRU = (1<<3),
	CACHEDISABLE = (1<<4),
	ACCESSED = (1<<5),
	RESERVED0 = (1<<6),
	HUGE_PAGE = (1<<7),
	GLOBAL = (1<<8)
};

inline
uintptr_t operator|(PageFlags a, PageFlags b)
{
	return (static_cast<uintptr_t>(a) | static_cast<uintptr_t>(b));
}

inline
uintptr_t operator|(uintptr_t a, PageFlags b)
{
	return (a | static_cast<uintptr_t>(b));
}

inline
uintptr_t operator&(PageFlags a, PageFlags b)
{
	return (static_cast<uintptr_t>(a) & static_cast<uintptr_t>(b));
}

inline
uintptr_t operator&(uintptr_t a, PageFlags b)
{
	return (a & static_cast<uintptr_t>(b));
}


template<class Uint, const unsigned PAGE_SIZE=4096u>
class Frames
{
public:
	Frames() = default;
	Frames(Uint maxSize)	// maxSize == max number of bytes
		:frames{nullptr}	// pointer to buffer
		, count{0}		// number of frames (pages, bits )
	{
		//~ printf32("Frames::Frames: maxSize == %d\n", (uint32_t)maxSize);
		count = (maxSize >> 12);	// maxSize / 4096 == count pages
		auto numUints = INDEX_FROM_BIT(count) + 1;	// number of Uint's in the Frames == sizeof(Uint)*8 bits
		auto allocSize = numUints*sizeof(Uint);			// number of bytes to allocate == allocSize*8 bits
		frames = (Uint*)kmalloc_aligned(allocSize);		// frames buffer.

		printf("maxzSize: %u\n",(uint32_t)maxSize);
		
		printf("frames: %016.16lx to %016.16lx for %ld bits, maxSize: %lu\n", (uint64_t)frames, (uint64_t)(((unsigned char *)frames) + allocSize), allocSize*8, (uint64_t)maxSize);
		memset(frames, 0, allocSize);
		printf("maxzSize: %u\n",(uint32_t)maxSize);

	}

	void markFrames(Uint addr, size_t count)
	{
		for( auto i=0u; i<count; ++i, addr+=PAGE_SIZE)
		{
			set(addr);
		}
	}

	void mapInitialPages(Uint initialMappedPages)
	{
		markFrames(0, initialMappedPages);
		// // mark the pages that are already mapped as in use.
		// uint64_t addr = 0;
		// char buffer[32];
		// for( auto i=0; i<initialMappedPages; ++i, addr+=PAGE_SIZE)
		// {
		// 	// sprintf(buffer,"%d\n",i);
		// 	// debug_out(buffer);
		// 	set(addr);
		// }
	}

	void alloc(PageT<> *page, uintptr_t flags)
	{
		if( page->frame == 0 )
		{
			Uint offset = 0;
			Uint index = first(offset);
			if( index == static_cast<Uint>(-1) )
			{
				PANIC("No free frames!");
			}
			
			ASSERT( INDEX_FROM_BIT(index*8*sizeof(Uint)+offset) == index );
			ASSERT( OFFSET_FROM_BIT(index*8*sizeof(Uint)+offset) == offset );
			
			// ++nAllocated;
			// set(idx*0x1000);
			
			frames[index] |= (((Uint)0x1u) << offset);
			page->present = flags & PageFlags::PRESENT ? 1 : 0;
			page->rw = flags & PageFlags::READWRITE ? 1 : 0;
			page->user = flags & PageFlags::USER ? 1 : 0;
			page->write_through = flags & PageFlags::WRITETHRU ? 1 : 0;
			page->no_cache = flags & PageFlags::CACHEDISABLE ? 1 : 0;
			page->dirty = 0;
			page->huge = flags & PageFlags::HUGE_PAGE ? 1 : 0;
			page->global = flags & PageFlags::GLOBAL ? 1 : 0;
			page->bit9 = page->bit10 = page->bit11 = 0;
			uintptr_t fr = index*sizeof(Uint)*8+offset;
			page->frame = fr;
		}

	}

	void alloc(PageT<> *page, int isKernel, int isWritable)
	{
		// static unsigned int nAllocated = 0;
		if( page->frame == 0 )
		{
			Uint offset = 0;
			Uint index = first(offset);
			if( index == static_cast<Uint>(-1) )
			{
				PANIC("No free frames!");
			}
			
			ASSERT( INDEX_FROM_BIT(index*8*sizeof(Uint)+offset) == index );
			ASSERT( OFFSET_FROM_BIT(index*8*sizeof(Uint)+offset) == offset );
			
			// ++nAllocated;
			// set(idx*0x1000);
			
			frames[index] |= (((Uint)0x1u) << offset);
			page->present = 1;
			page->rw = (isWritable ? 1 : 0);
			page->user = (isKernel ? 0 : 1);
			uintptr_t fr = index*sizeof(Uint)*8+offset;
			page->frame = fr;
		}
	}

	void free(PageT<> *page)
	{
		auto frame = page->frame;
		if( frame != 0 )
		{
			clear(frame);
			page->frame = 0;
			(*page) = PageT<>{};
		}
	}

private:

	static Uint INDEX_FROM_BIT( Uint a)
	{
		// given a bit (page) index, return the index of the Uint that contains the bit.
		const auto divisor = (8 * sizeof(Uint));
		return (a / divisor);
	}

	static Uint OFFSET_FROM_BIT( Uint a)
	{
		const auto divisor = (8 * sizeof(Uint));
		return a % divisor;
	}

	void set(Uint addr)
	{
		auto frame = addr / PAGE_SIZE;
		auto idx = INDEX_FROM_BIT(frame);
		auto off = OFFSET_FROM_BIT(frame);
		frames[idx] |= (((Uint)0x1u) << off);
	}

	void clear(Uint addr)
	{
		debug_out("addr:0x%016.16x\n",addr);
		auto frame = addr / PAGE_SIZE;
		auto idx = INDEX_FROM_BIT(frame);
		auto off = OFFSET_FROM_BIT(frame);
		frames[idx] &= ~(((Uint)0x1u) << off);
	}

	Uint test(Uint addr)
	{
		auto frame = addr / PAGE_SIZE;
		auto idx = INDEX_FROM_BIT(frame);
		auto off = OFFSET_FROM_BIT(frame);
		return (frames[idx] & (((Uint)0x1u) << off));
	}

	// return index into frames array and set offset to bit number inside frames[index]
	Uint first(Uint &offset)
	{
		static uint32_t numFound = 0;
		const auto ie = INDEX_FROM_BIT(count);
		const auto je = sizeof(Uint) * 8;
		for (auto i = 0u; i <= ie; ++i)
		{
			auto frame = frames[i];
			if (frame != static_cast<Uint>(-1)) // nothing free, exit early.
			{
				// at least one bit is free here.
				for (auto j = 0u; j < je; ++j)
				{
					Uint toTest = ( (Uint{0x1u}) << j);
					if ( 0 == (frame & toTest) )
					{
						++numFound;
						offset = j;
						return i;
						// index = i;
						// return i*sizeof(Uint)*8+j;
					}
				}
			}
		}
		return (Uint)(-1);
	}

private:
	Uint *	frames{nullptr};
	uint32_t	count{0};
};


#endif // PAGE_FRAMES_H_INCLUDED
