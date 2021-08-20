#ifndef KMALLOC_H_INCLUDED
#define KMALLOC_H_INCLUDED

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

void *kmalloc(size_t sz);
void *kmalloc_aligned(size_t sz);
void *kmalloc_phys(size_t sz, void **phys);
void *kmalloc_aligned_phys(size_t sz, void **phys);

void *krealloc(void *p, size_t size);

void kfree(void *vaddr);



#ifdef __cplusplus
}
#endif



template<class T>
struct KAllocator
{
    using value_type=T;
    KAllocator() = default;
	template <class U> constexpr KAllocator(const KAllocator<U>&) noexcept
	{

	}
	[[nodiscard]]
	T* allocate(std::size_t n)noexcept 
	{
		if(n > std::size_t(-1) / sizeof(T)) 
		{
			return nullptr;
		}
		return static_cast<T*>(kmalloc(n*sizeof(T)));
	}

	void deallocate(T* p, std::size_t) noexcept 
	{
		if( p != nullptr)
		{
			kfree(p); 
		}
	}
};

#endif // KMALLOC_H_INCLUDED
