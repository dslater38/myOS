#ifndef NEWOBJ_H__INCLUDED
#define NEWOBJ_H__INCLUDED

#include "new.h"
#include "kmalloc.h"
#include <utility>


template<typename T, typename... TArgs>
T *New(TArgs... args)noexcept
{
	return new(kmalloc(sizeof(T))) T{ std::forward<TArgs>(args)... };
}

template<typename T, typename... TArgs>
T *AlignedNew(TArgs... args)noexcept
{
	return new(kmalloc_aligned(sizeof(T))) T{ std::forward<TArgs>(args)... };
}

template<typename T, typename... TArgs>
T *AlignedNewPhys(void **phys, TArgs... args)noexcept
{
	return new(kmalloc_aligned_phys(sizeof(T), phys)) T{ std::forward<TArgs>(args)... };
}


#endif // NEWOBJ_H__INCLUDED