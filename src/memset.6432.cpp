#include "common.h"

extern "C"
{

// Copy len bytes from src to dest.
STATIC32 void *SYM6432(memcpy)(void *dest, const void *src, size_t len)
{
    const uint8_t *sp = (const uint8_t *)src;
    uint8_t *dp = (uint8_t *)dest;
    for(; len != 0; len--) *dp++ = *sp++;
     return dest;
}

// Write len copies of val into dest.
void * SYM6432(memset)(void *vdest, int val, size_t len)
{
#ifndef  FAST_MEMCPY
	uint8_t *ptr,*pEnd;
	for( ptr = (uint8_t *)vdest, pEnd = ptr + len;
		ptr<pEnd;
		++ptr )
	{
		*ptr = (uint8_t)val;
	}
	return vdest;
#else
	uint8_t *dest = (uint8_t *)vdest;
	uint32_t slen,dlen;
	
	dlen = (len >> 2);	/* length in DWORDS*/
	
	if(dlen>0)
	{
		uint32_t v = ((val<<24)|(val<<16)|(val<<8)|val);
		uint32_t *ptr = (uint32_t *)dest;
		dest = (uint8_t *)(ptr + dlen);
		for( ;ptr < (uint32_t *)dest; ++ptr )
		{
			*ptr = v;
		}
		len -= (dlen << 2);
	}
	
	slen = (len>>1);
	if(slen>0)
	{
		uint16_t v = ((val<<8)|val);
		uint16_t *ptr = (uint16_t *)dest;
		dest = (uint8_t *)(ptr + slen);
		for( ;ptr < (uint16_t *)dest; ++ptr )
		{
			*ptr = v;
		}
		len -= (slen << 1);
	}
	
	if(len>0)
	{
		uint8_t *ptr = dest;
		dest = (ptr + len);
		for( ;ptr < dest; ++ptr )
		{
			*ptr = val;
		}
		
	}
#endif
}

}
