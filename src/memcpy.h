#ifndef MEMCPY_H_INCLUDED__
#define MEMCPY_H_INCLUDED__

#include <string.h>

#if 0

#ifndef __cplusplus

template<class T>
T *memcpy(T *src, T *dst, size_t count )
{
	auto *retVal = dst;
	for(auto pEnd=src+count;src<pEnd;++src,++dst)
	{
		*dst = *src;
	}
	return retVal;
}

#else
extern "C"
{

#endif

void *memset(void *dest, uint8_t val, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif // 0

#endif // MEMCPY_H_INCLUDED__

