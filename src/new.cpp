#include "new.h"
extern "C" {
	
#include "kmalloc.h"
	
}


void *operator new(std::size_t size)
{
	return reinterpret_cast<void *>(kmalloc(size));
}

void operator delete(void *)noexcept
{
	// kfree(reinterpret_cast<>);
}

void *operator new[](std::size_t size)
{
	return reinterpret_cast<void *>(kmalloc(size));
}

void operator delete[](void *)noexcept
{
	// kfree(reinterpret_cast<>);
}
