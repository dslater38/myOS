#include "new.h"
extern "C" {
	
#include "kmalloc.h"
	
}
	void *operator new(std::size_t size, const std::nothrow_t &)noexcept
	{
		return kmalloc(size);
	}

	void operator delete(void *p, const std::nothrow_t &)noexcept
	{
		kfree(0);
	}

	void *operator new[](std::size_t size, const std::nothrow_t &)noexcept
	{
		return kmalloc(size);
	}

	void operator delete[](void *p, const std::nothrow_t &)noexcept
	{
		kfree(0);
	}


namespace std
{
	const nothrow_t nothrow{};
}