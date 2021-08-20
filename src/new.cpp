#include "new.h"
#include "kmalloc.h"

	void *operator new(std::size_t size, const std::nothrow_t &)noexcept
	{
		return kmalloc(size);
	}

	void operator delete(void *p, const std::nothrow_t &)noexcept
	{
		kfree(p);
	}

	void *operator new[](std::size_t size, const std::nothrow_t &)noexcept
	{
		return kmalloc(size);
	}

	void operator delete[](void *p, const std::nothrow_t &)noexcept
	{
		kfree(p);
	}

	void *operator new(std::size_t size)
	{
		return kmalloc(size);
	}

	void operator delete(void *p)
	{
		kfree(p);
	}

	void *operator new[](std::size_t size)
	{
		return kmalloc(size);
	}

	void operator delete[](void *p)
	{
		kfree(p);
	}

	static void *aligned_alloc__(size_t size, size_t align)
	{
		auto offset = align - 1 + sizeof(void *);
		auto *p1 = reinterpret_cast<void *>(kmalloc(size + offset));
		auto *p2=reinterpret_cast<void**>((reinterpret_cast<size_t>(p1)+offset)&~(align-1));
		p2[-1] = p1;
		return p2;
	}

	static void aligned_free(void *p)
	{
		if( p )
		{
			auto *p2 = reinterpret_cast<void**>(p);
			auto *p1 = p2[-1];
			kfree(p1);
		}
	}

	void *operator new(std::size_t size, const std::align_val_t align)
	{
		return aligned_alloc__(size, static_cast <size_t >(align));
	}

	void operator delete(void *p, const std::align_val_t )
	{
		aligned_free(p);
	}

	void *operator new[](std::size_t size, const std::align_val_t align)
	{
		return aligned_alloc__(size, static_cast <size_t >(align));
	}

	void operator delete[](void *p, const std::align_val_t )
	{
		aligned_free(p);
	}




namespace std
{
	void __throw_length_error(const char *message)
	{
		PANIC(message);
	}
	const nothrow_t nothrow{};
}