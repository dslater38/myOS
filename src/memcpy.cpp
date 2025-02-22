/********************************************************************
 ** File:     memcpy.c
 **
 ** Copyright (C) 1999-2010 Daniel Vik
 ** 
 ** This software is provided 'as-is', without any express or implied
 ** warranty. In no event will the authors be held liable for any
 ** damages arising from the use of this software.
 ** Permission is granted to anyone to use this software for any
 ** purpose, including commercial applications, and to alter it and
 ** redistribute it freely, subject to the following restrictions:
 ** 
 ** 1. The origin of this software must not be misrepresented; you
 **    must not claim that you wrote the original software. If you
 **    use this software in a product, an acknowledgment in the
 **    use this software in a product, an acknowledgment in the
 **    product documentation would be appreciated but is not
 **    required.
 ** 
 ** 2. Altered source versions must be plainly marked as such, and
 **    must not be misrepresented as being the original software.
 ** 
 ** 3. This notice may not be removed or altered from any source
 **    distribution.
 ** 
 ** 
 ** Description: Implementation of the standard library function memcpy.
 **             This implementation of memcpy() is ANSI-C89 compatible.
 **
 **             The following configuration options can be set:
 **
 **           LITTLE_ENDIAN   - Uses processor with little endian
 **                             addressing. Default is big endian.
 **
 **           PRE_INC_PTRS    - Use pre increment of pointers.
 **                             Default is post increment of
 **                             pointers.
 **
 **           INDEXED_COPY    - Copying data using array indexing.
 **                             Using this option, disables the
 **                             PRE_INC_PTRS option.
 **
 **           MEMCPY_64BIT    - Compiles memcpy for 64 bit 
 **                             architectures
 **
 **
 ** Best Settings:
 **
 ** Intel x86:  LITTLE_ENDIAN and INDEXED_COPY
 **
 *******************************************************************/



/********************************************************************
 ** Configuration definitions.
 *******************************************************************/

#define LITTLE_ENDIAN
#define INDEXED_COPY
#define MEMCPY_64BIT


/********************************************************************
 ** Includes for size_t definition
 *******************************************************************/

#include <stddef.h>
#include <stdint.h>


/********************************************************************
 ** Typedefs
 *******************************************************************/

//~ typedef unsigned char       uint8_t;
//~ typedef unsigned short      uint16_t;
//~ typedef unsigned int        uint32_t;
//~ #ifdef _WIN32
//~ typedef unsigned __int64    uint64_t;
//~ #else
//~ typedef unsigned long long  uint64_t;
//~ #endif

#ifdef MEMCPY_64BIT
// typedef uint64_t              uintptr_t;
#define TYPE_WIDTH          8L
#else
// typedef uint32_t              uintptr_t;
#define TYPE_WIDTH          4L
#endif


/********************************************************************
 ** Remove definitions when INDEXED_COPY is defined.
 *******************************************************************/

#if defined (INDEXED_COPY)
#if defined (PRE_INC_PTRS)
#undef PRE_INC_PTRS
#endif /*PRE_INC_PTRS*/
#endif /*INDEXED_COPY*/



/********************************************************************
 ** Definitions for pre and post increment of pointers.
 *******************************************************************/

#if defined (PRE_INC_PTRS)

#define START_VAL(x)            (x)--
#define INC_VAL(x)              *++(x)
#define CAST_TO_U8(p, o)        ((uint8_t*)p + o + TYPE_WIDTH)
#define WHILE_DEST_BREAK        (TYPE_WIDTH - 1)
#define PRE_LOOP_ADJUST         - (TYPE_WIDTH - 1)
#define PRE_SWITCH_ADJUST       + 1

#else /*PRE_INC_PTRS*/

#define START_VAL(x)
#define INC_VAL(x)              *(x)++
#define CAST_TO_U8(p, o)        ((uint8_t*)p + o)
#define WHILE_DEST_BREAK        0
#define PRE_LOOP_ADJUST
#define PRE_SWITCH_ADJUST

#endif /*PRE_INC_PTRS*/



/********************************************************************
 ** Definitions for endians
 *******************************************************************/

#if defined (LITTLE_ENDIAN)

#define SHL >>
#define SHR <<

#else /* LITTLE_ENDIAN */

#define SHL <<
#define SHR >>

#endif /* LITTLE_ENDIAN */



/********************************************************************
 ** Macros for copying words of  different alignment.
 ** Uses incremening pointers.
 *******************************************************************/

#define CP_INCR() {                         \
    INC_VAL(dstN) = INC_VAL(srcN);          \
}

#define CP_INCR_SH(shl, shr) {              \
    dstWord   = srcWord SHL shl;            \
    srcWord   = INC_VAL(srcN);              \
    dstWord  |= srcWord SHR shr;            \
    INC_VAL(dstN) = dstWord;                \
}



/********************************************************************
 ** Macros for copying words of  different alignment.
 ** Uses array indexes.
 *******************************************************************/

#define CP_INDEX(idx) {                     \
    dstN[idx] = srcN[idx];                  \
}

#define CP_INDEX_SH(x, shl, shr) {          \
    dstWord   = srcWord SHL shl;            \
    srcWord   = srcN[x];                    \
    dstWord  |= srcWord SHR shr;            \
    dstN[x]  = dstWord;                     \
}



/********************************************************************
 ** Macros for copying words of different alignment.
 ** Uses incremening pointers or array indexes depending on
 ** configuration.
 *******************************************************************/

#if defined (INDEXED_COPY)

#define CP(idx)               CP_INDEX(idx)
#define CP_SH(idx, shl, shr)  CP_INDEX_SH(idx, shl, shr)

#define INC_INDEX(p, o)       ((p) += (o))

#else /* INDEXED_COPY */

#define CP(idx)               CP_INCR()
#define CP_SH(idx, shl, shr)  CP_INCR_SH(shl, shr)

#define INC_INDEX(p, o)

#endif /* INDEXED_COPY */


#define COPY_REMAINING(count) {                                     \
    START_VAL(dst8);                                                \
    START_VAL(src8);                                                \
                                                                    \
    switch (count) {                                                \
    case 7: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 6: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 5: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 4: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 3: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 2: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 1: INC_VAL(dst8) = INC_VAL(src8);                          \
    case 0:                                                         \
    default: break;                                                 \
    }                                                               \
}

#define COPY_NO_SHIFT() {                                           \
    uintptr_t* dstN = (uintptr_t*)(dst8 PRE_LOOP_ADJUST);                   \
    uintptr_t* srcN = (uintptr_t*)(src8 PRE_LOOP_ADJUST);                   \
    size_t length = count / TYPE_WIDTH;                             \
                                                                    \
    while (length & 7) {                                            \
        CP_INCR();                                                  \
        length--;                                                   \
    }                                                               \
                                                                    \
    length /= 8;                                                    \
                                                                    \
    while (length--) {                                              \
        CP(0);                                                      \
        CP(1);                                                      \
        CP(2);                                                      \
        CP(3);                                                      \
        CP(4);                                                      \
        CP(5);                                                      \
        CP(6);                                                      \
        CP(7);                                                      \
                                                                    \
        INC_INDEX(dstN, 8);                                         \
        INC_INDEX(srcN, 8);                                         \
    }                                                               \
                                                                    \
    src8 = CAST_TO_U8(srcN, 0);                                     \
    dst8 = CAST_TO_U8(dstN, 0);                                     \
                                                                    \
    COPY_REMAINING(count & (TYPE_WIDTH - 1));                       \
                                                                    \
    return dest;                                                    \
}



#define COPY_SHIFT(shift) {                                         \
    uintptr_t* dstN  = (uintptr_t*)((((uintptr_t)dst8) PRE_LOOP_ADJUST) &       \
                             ~(TYPE_WIDTH - 1));                    \
    uintptr_t* srcN  = (uintptr_t*)((((uintptr_t)src8) PRE_LOOP_ADJUST) &       \
                             ~(TYPE_WIDTH - 1));                    \
    size_t length  = count / TYPE_WIDTH;                            \
    uintptr_t srcWord = INC_VAL(srcN);                                  \
    uintptr_t dstWord;                                                  \
                                                                    \
    while (length & 7) {                                            \
        CP_INCR_SH(8 * shift, 8 * (TYPE_WIDTH - shift));            \
        length--;                                                   \
    }                                                               \
                                                                    \
    length /= 8;                                                    \
                                                                    \
    while (length--) {                                              \
        CP_SH(0, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(1, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(2, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(3, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(4, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(5, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(6, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
        CP_SH(7, 8 * shift, 8 * (TYPE_WIDTH - shift));              \
                                                                    \
        INC_INDEX(dstN, 8);                                         \
        INC_INDEX(srcN, 8);                                         \
    }                                                               \
                                                                    \
    src8 = CAST_TO_U8(srcN, (shift - TYPE_WIDTH));                  \
    dst8 = CAST_TO_U8(dstN, 0);                                     \
                                                                    \
    COPY_REMAINING(count & (TYPE_WIDTH - 1));                       \
                                                                    \
    return dest;                                                    \
}


/********************************************************************
 **
 ** void *memcpy(void *dest, const void *src, size_t count)
 **
 ** Args:     dest        - pointer to destination buffer
 **           src         - pointer to source buffer
 **           count       - number of bytes to copy
 **
 ** Return:   A pointer to destination buffer
 **
 ** Purpose:  Copies count bytes from src to dest. 
 **           No overlap check is performed.
 **
 *******************************************************************/

extern "C"
{

	void *memcpy(void *dest, const void *src, size_t count) noexcept
	{
		uint8_t* dst8 = (uint8_t*)dest;
		uint8_t* src8 = (uint8_t*)src;

		if (count < 8) {
			COPY_REMAINING(count);
			return dest;
		}

		START_VAL(dst8);
		START_VAL(src8);

		while (((uintptr_t)dst8 & (TYPE_WIDTH - 1)) != WHILE_DEST_BREAK) {
			INC_VAL(dst8) = INC_VAL(src8);
			count--;
		}

		switch ( (((uintptr_t)src8) PRE_SWITCH_ADJUST) & (TYPE_WIDTH - 1) ) {
		case 0: COPY_NO_SHIFT(); break;
		case 1: COPY_SHIFT(1);   break;
		case 2: COPY_SHIFT(2);   break;
		case 3: COPY_SHIFT(3);   break;
#if TYPE_WIDTH > 4
		case 4: COPY_SHIFT(4);   break;
		case 5: COPY_SHIFT(5);   break;
		case 6: COPY_SHIFT(6);   break;
		case 7: COPY_SHIFT(7);   break;
#endif
	    }
	    
	    return dest;
	}
	
	// dst_index is the index of the destination page (vaddr == dst_index<<12)
	// src_index is the index of the destination page (vaddr == src_index<<12)
	// page_count is the number of 4K pages to copy
	void page_copy(uint64_t dst_index, uint64_t src_index, size_t page_count)noexcept
	{		
		auto *dstN = reinterpret_cast<uint64_t *>(dst_index<<12);
		const auto *srcN = reinterpret_cast<const uint64_t *>(src_index<<12);
//		auto qword_count = (page_count << 9); // there are 512 qwords in a 4096 byte page, so multiply page_count by 512
//		auto length = qword_count / 8;	// unroll the loop. 
        auto length = (page_count << 6); // (page_count << 9)/8 == (page_count << 6 )
		while(length--)
		{
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
			*dstN++ = *srcN++;
		}
	}
}
