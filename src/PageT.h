#ifndef PAGET_H_INCLUDED
#define PAGET_H_INCLUDED


#include "common.h"


template<class UINT>
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

	UINT present : 1;
	UINT rw : 1;
	UINT user : 1;
	UINT accessed : 1;
	UINT dirty : 1;
	UINT unused : 7;
	UINT frame : (8*sizeof(UINT) - 12);
};

#endif // PAGET_H_INCLUDED
