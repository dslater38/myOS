#ifndef PAGET_H_INCLUDED
#define PAGET_H_INCLUDED


#include "common.h"


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
	
	using  Pointer=UINT;
	static constexpr size_t OFFBITS=OFFBITS_;
	static constexpr size_t offset_bits=OFFBITS;
	static constexpr size_t shift=OFFBITS;
	static constexpr bool is_huge=(OFFBITS > 12);
	
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
	
	PageT<UINT, OFFBITS> *getPage()const
	{
		return this;
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
