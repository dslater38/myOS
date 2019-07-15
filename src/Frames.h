#ifndef PAGE_FRAMES_H_INCLUDED
#define PAGE_FRAMES_H_INCLUDED

#include "common.h"
#include "PageT.h"

template<class UINT, const unsigned PAGE_SIZE=4096u>
class Frames
{
public:
	Frames() = default;
	Frames(UINT maxSize)
		:frames{(UINT*)kmalloc(INDEX_FROM_BIT((maxSize>>12)))}
		, count{static_cast<u32int>(maxSize>>12)}
	{
		memset(frames, 0, INDEX_FROM_BIT(count));
	}
	

	void alloc(PageT<UINT> *page, int isKernel, int isWritable)
	{
		if( page->frame == 0 )
		{
			auto idx = first();
			if( idx == static_cast<UINT>(-1) )
			{
				PANIC("No free frames!");
			}
			set(idx*0x1000);
			page->present = 1;
			page->rw = (isWritable ? 1 : 0);
			page->user = (isKernel ? 0 : 1);
			page->frame = idx;
		}
	}

	void free(PageT<UINT> *page)
	{
		auto frame = page->frame();
		if( frame != 0 )
		{
			clear(frame);
			page->frame = 0;
		}
	}
	
private:
	
	static UINT INDEX_FROM_BIT( UINT a)
	{
		return a / (8*sizeof(UINT));
	}
	
	static UINT OFFSET_FROM_BIT( UINT a)
	{
		return a % (8*sizeof(UINT));
	}
	
	void set(UINT addr)
	{
		auto frame = addr / PAGE_SIZE;
		auto idx = INDEX_FROM_BIT(frame);
		auto off = OFFSET_FROM_BIT(frame);
		frames[idx] |= (0x1 << off);
	}
	
	void clear(UINT addr)
	{
		auto frame = addr / PAGE_SIZE;
		auto idx = INDEX_FROM_BIT(frame);
		auto off = OFFSET_FROM_BIT(frame);
		frames[idx] &= ~(0x1 << off);
	}
	
	UINT test(UINT addr)
	{
		auto frame = addr / PAGE_SIZE;
		auto idx = INDEX_FROM_BIT(frame);
		auto off = OFFSET_FROM_BIT(frame);
		return (frames[idx] & (0x1 << off));
	}
	
	UINT first()
	{
		for (auto i = 0u; i < INDEX_FROM_BIT(count); ++i)
		{
			auto frame = frames[i];
			if (frame != static_cast<UINT>(-1)) // nothing free, exit early.
			{			
				// at least one bit is free here.
				for (auto j = 0u; j < sizeof(UINT)*8; ++j)
				{
					auto toTest = (static_cast<UINT>(0x1) << j);
					if ( 0 == (frame & toTest) )
					{
						return i*sizeof(UINT)*8+j;
					}
				}
			}
		}
		return (UINT)(-1);
	}
	
private:
	UINT *	frames{nullptr};
	u32int	count{0};
};


#endif // PAGE_FRAMES_H_INCLUDED
