#ifndef PAGET_H_INCLUDED
#define PAGET_H_INCLUDED


#include "common.h"

#if 0
template<class UINT>
struct PageTableBase
{
	using Pointer=UINT;
	Pointer table[4098/sizeof(Pointer)];
};

template<class UINT, const size_t BITS>
class PageEntryTraits
{
};

template<class UINT>
class PageEntryTraits<UINT, 12>
{
	static constexpr size_t FRAME_BITS=sizeof(UINT)-12;
	static constexpr size_t INDEX_BITS=0;
};

template<class UINT>
class PageEntryTraits<UINT, 21>
{
	static constexpr size_t FRAME_BITS=sizeof(UINT)-12;
	static constexpr size_t INDEX_BITS=9;
	static constexpr size_t BIG_FRAME_BITS=sizeof(UINT)-21;
};

template<class UINT>
class PageEntryTraits<UINT, 22>
{
	static constexpr size_t FRAME_BITS=sizeof(UINT)-12;
	static constexpr size_t INDEX_BITS=10;
	static constexpr size_t BIG_FRAME_BITS=sizeof(UINT)-22;
};

template<class UINT>
class PageEntryTraits<UINT, 30>
{
	static constexpr size_t FRAME_BITS=sizeof(UINT)-12;
	static constexpr size_t INDEX_BITS=9;
	static constexpr size_t BIG_FRAME_BITS=sizeof(UINT)-30;
};

template<class UINT>
class PageEntryTraits<UINT, 30>
{
	static constexpr size_t FRAME_BITS=sizeof(UINT)-12;
	static constexpr size_t INDEX_BITS=9;
	static constexpr size_t BIG_FRAME_BITS=sizeof(UINT)-30;
};

template<class UINT>
class PageEntryTraits<UINT, 39>
{
	static constexpr size_t FRAME_BITS=sizeof(UINT)-12;
	static constexpr size_t INDEX_BITS=9;
	static constexpr size_t BIG_FRAME_BITS=0;
};

template<class UINT, const size_t BITS>
class PageEntry
{
	using  Pointer=UINT;
	Pointer present : 1;
	Pointer rw : 1;
	Pointer user : 1;
	Pointer write_through : 1;
	Pointer no_cache : 1;
	Pointer accessed : 1;
	Pointer dirty : 1;
	Pointer huge : 1;	// unused  - must be 0
	Pointer global : 1;	// ignored
	Pointer bit9 : 1;
	Pointer bit10 : 1;
	Pointer bit11 : 1;
	Pointer frame : (8*sizeof(Pointer) - BITS);

};
#endif // 0

template<class UINT, const size_t OFFBITS_=12>
struct PageT
{
	//~ static constexpr UINT PRESENT_BIT = (UINT{1})<<0;
	//~ static constexpr UINT RW_BIT = (UINT{1})<<1;
	//~ static constexpr UINT USER_BIT = (UINT{1})<<2;
	//~ static constexpr UINT ACCESSED_BIT = (UINT{1})<<3;
	//~ static constexpr UINT DIRTY_BIT = (UINT{1})<<4;
	//~ static constexpr UINT UNUSED_BITS = (UINT{0x7F}) << 5;
	//~ static constexpr UINT FRAME_BITS = (static_cast<UINT>(-1) << 12);
	
	PageT()
	: present{0}
	, rw{0}
	, user{0}
	, write_through{0}
	, no_cache{0}
	, accessed{0}
	, dirty{0}
	, huge{0}
	, global{0}
	, bit9{0}
	, bit10{0}
	, bit11{0}
	, frame{0}
	{

	}

	PageT(const PageT<UINT,OFFBITS_> &)=default;
	PageT<UINT,OFFBITS_> & operator=(const PageT<UINT,OFFBITS_> &)=default;
	PageT(PageT<UINT,OFFBITS_> &&)=default;
	PageT<UINT,OFFBITS_> & operator=(PageT<UINT,OFFBITS_> &&)=default;
	~PageT()=default;

	using  Pointer=UINT;
	static constexpr size_t OFFBITS=OFFBITS_;
	static constexpr size_t OFFSET_BITS=OFFBITS;
	static constexpr size_t offset_bits=OFFBITS;
	static constexpr size_t shift=OFFBITS;
	static constexpr bool is_huge=(OFFBITS > 12);
	// static constexpr PAGE_SIZE = (1<<OFFBITS);
	
	using PageType=PageT<UINT,OFFBITS>;
	

	Pointer present : 1;
	Pointer rw : 1;
	Pointer user : 1;
	Pointer write_through : 1;
	Pointer no_cache : 1;
	Pointer accessed : 1;
	Pointer dirty : 1;
	Pointer huge : 1;	// unused  - must be 0
	Pointer global : 1;	// ignored
	Pointer bit9 : 1;
	Pointer bit10 : 1;
	Pointer bit11 : 1;
	Pointer frame : (8*sizeof(Pointer) - OFFBITS);
	
	static PageT<Pointer> &toEntry(Pointer &n)
	{
		return *((PageT<Pointer> *)(&n));
	}
	
	PageT<UINT, OFFBITS> *getPage(Pointer )
	{
		return this;
	}
	
	const PageT<UINT, OFFBITS> *getPage(Pointer )const
	{
		return this;
	}
	
	void dump()const
	{
#if 1
		// printf("\tPage Entry: 0x%016.16lx\n", *(uint64_t *)(this));
#endif // 1
	}

	void setPhys(UINT){}
};

template<class UINT>
using Page4K = PageT<UINT,12>;

template<class UINT>
using Page2M = PageT<UINT, 21>;

using Page4M = PageT<uint32_t, 22>;

template<class UINT>
using Page1G = PageT<UINT, 30>;

template<class UINT>
struct PageDirectoryEntryT
{
	using  Pointer=UINT;
	
	Pointer present : 1;
	Pointer rw : 1;
	Pointer user : 1;
	Pointer write_through : 1;
	Pointer no_cache : 1;
	Pointer dirty : 1;
	Pointer huge : 1;
	Pointer global : 1;	// ignored
	Pointer bit9 : 1;
	Pointer bit10 : 1;
	Pointer bit11 : 1;
	Pointer frame : (8*sizeof(Pointer) - 12);
	
	static PageDirectoryEntryT<Pointer> &toEntry(Pointer &n)
	{
		return *((PageDirectoryEntryT<Pointer> *)(&n));
	}
	
};

#endif // PAGET_H_INCLUDED
