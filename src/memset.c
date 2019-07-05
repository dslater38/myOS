#include "common.h"

// Copy len bytes from src to dest.
void memcpy(void *dest, const void *src, u32int len)
{
    const u8int *sp = (const u8int *)src;
    u8int *dp = (u8int *)dest;
    for(; len != 0; len--) *dp++ = *sp++;
}

// Write len copies of val into dest.
void memset(void *vdest, u8int val, u32int len)
{
	u8int *dest = (u8int *)vdest;
	u32int slen,dlen;
	
	dlen = (len >> 2);	/* length in DWORDS*/
	
	if(dlen>0)
	{
		u32int v = ((val<<24)|(val<<16)|(val<<8)|val);
		u32int *ptr = (u32int *)dest;
		dest = (u8int *)(ptr + dlen);
		for( ;ptr < dest; ++ptr )
		{
			*ptr = v;
		}
		len -= (dlen << 2);
	}
	
	slen = (len>>1);
	if(slen>0)
	{
		u16int v = ((val<<8)|val);
		u16int *ptr = (u16int *)dest;
		dest = (u8int *)(ptr + slen);
		for( ;ptr < dest; ++ptr )
		{
			*ptr = v;
		}
		len -= (slen << 1);
	}
	
	if(len>0)
	{
		u8int *ptr = dest;
		dest = (ptr + len);
		for( ;ptr < dest; ++ptr )
		{
			*ptr = val;
		}
		
	}
}
