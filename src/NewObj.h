#ifndef NEWOBJ_H__INCLUDED
#define NEWOBJ_H__INCLUDED

#include "new.h"
#include "kmalloc.h"
#include <utility>


template<typename T, typename... TArgs>
T *New(TArgs... args)noexcept
{
	return new(std::nothrow) T{ std::forward<TArgs>(args)... };
}

template<typename T, typename... TArgs>
T *AlignedNew(TArgs... args)noexcept
{
	void *memory = kmalloc_aligned(sizeof(T));
	return new(memory) T{ std::forward<TArgs>(args)... };
}

template<typename T, typename... TArgs>
T *AlignedNewPhys(void **phys, TArgs... args)noexcept
{
	void *memory = kmalloc_aligned_phys(sizeof(T), phys);
	return new(memory) T{ std::forward<TArgs>(args)... };
}


#endif // NEWOBJ_H__INCLUDED