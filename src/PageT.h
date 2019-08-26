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

template<const size_t OFFBITS_=12>
struct PageT
{
	//~ static constexpr uintptr_t PRESENT_BIT = (uintptr_t{1})<<0;
	//~ static constexpr uintptr_t RW_BIT = (uintptr_t{1})<<1;
	//~ static constexpr uintptr_t USER_BIT = (uintptr_t{1})<<2;
	//~ static constexpr uintptr_t ACCESSED_BIT = (uintptr_t{1})<<3;
	//~ static constexpr uintptr_t DIRTY_BIT = (uintptr_t{1})<<4;
	//~ static constexpr uintptr_t UNUSED_BITS = (uintptr_t{0x7F}) << 5;
	//~ static constexpr uintptr_t FRAME_BITS = (static_cast<uintptr_t>(-1) << 12);
	
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

	PageT(const PageT<OFFBITS_> &)=default;
	PageT<OFFBITS_> & operator=(const PageT<OFFBITS_> &)=default;
	PageT(PageT<OFFBITS_> &&)=default;
	PageT<OFFBITS_> & operator=(PageT<OFFBITS_> &&)=default;
	~PageT()=default;

	static constexpr size_t OFFBITS=OFFBITS_;
	static constexpr size_t OFFSET_BITS=OFFBITS;
	static constexpr size_t offset_bits=OFFBITS;
	static constexpr size_t shift=OFFBITS;
	static constexpr bool is_huge=(OFFBITS > 12);
	
	using PageType=PageT<OFFBITS>;
	

	uintptr_t present : 1;
	uintptr_t rw : 1;
	uintptr_t user : 1;
	uintptr_t write_through : 1;
	uintptr_t no_cache : 1;
	uintptr_t accessed : 1;
	uintptr_t dirty : 1;
	uintptr_t huge : 1;	// unused  - must be 0
	uintptr_t global : 1;	// ignored
	uintptr_t bit9 : 1;
	uintptr_t bit10 : 1;
	uintptr_t bit11 : 1;
	uintptr_t frame : (8*sizeof(uintptr_t) - OFFBITS);
	
	static PageT<OFFBITS_> &toEntry(uintptr_t &n)
	{
		return *((PageT<OFFBITS_> *)(&n));
	}
	
	PageT<OFFBITS> *getPage(uintptr_t )
	{
		return this;
	}
	
	PageT<OFFBITS> *findPage(uintptr_t )
	{
		return this;
	}
	
	const PageT<OFFBITS> *getPage(uintptr_t )const
	{
		return this;
	}
	
	const PageT<OFFBITS> *findPage(uintptr_t )const
	{
		return this;
	}
	
	void dump()const
	{
#if 1
		debug_out("\tPage Entry: 0x%016.16lx\n", *(uint64_t *)(this));
#endif // 1
	}

	void setPhys(uintptr_t){}
};

using Page4K = PageT<12>;

using Page2M = PageT<21>;

using Page4M = PageT<22>;

using Page1G = PageT<30>;

struct PageDirectoryEntryT
{
	
	uintptr_t present : 1;
	uintptr_t rw : 1;
	uintptr_t user : 1;
	uintptr_t write_through : 1;
	uintptr_t no_cache : 1;
	uintptr_t dirty : 1;
	uintptr_t huge : 1;
	uintptr_t global : 1;	// ignored
	uintptr_t bit9 : 1;
	uintptr_t bit10 : 1;
	uintptr_t bit11 : 1;
	uintptr_t frame : (8*sizeof(uintptr_t) - 12);
	
	static PageDirectoryEntryT &toEntry(uintptr_t &n)
	{
		return *((PageDirectoryEntryT *)(&n));
	}
	
};

#endif // PAGET_H_INCLUDED
