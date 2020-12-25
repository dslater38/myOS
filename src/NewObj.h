#ifndef NEWOBJ_H__INCLUDED
#define NEWOBJ_H__INCLUDED

#include "new.h"
#include "kmalloc.h"
#include <utility>
#include <memory>

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

template<class T>
struct nothrow_delete
{
	constexpr nothrow_delete() noexcept = default;

      /** @brief Converting constructor.
       *
       * Allows conversion from a deleter for arrays of another type, @p _Up,
       * only if @p _Up* is convertible to @p _Tp*.
       */
      template<typename U, typename = typename std ::enable_if<std ::is_convertible<U*, T*>::value>::type>
        nothrow_delete(const nothrow_delete<U>&) noexcept { }
	
	void operator()(T* ptr) const
	{
		delete(ptr, std::nothrow);
	}
};

template<class T>
struct nothrow_delete<T[]>
{
	constexpr nothrow_delete() noexcept = default;

      /** @brief Converting constructor.
       *
       * Allows conversion from a deleter for arrays of another type, @p _Up,
       * only if @p _Up* is convertible to @p _Tp*.
       */
      template<typename U, typename = typename std ::enable_if<std ::is_convertible<U(*)[], T(*)[]>::value>::type>
        nothrow_delete(const nothrow_delete<U[]>&) noexcept { }
	
	void operator()(T* ptr) const
	{
		delete[](ptr, std::nothrow);
	}
};

template<class T>
using uniquePtr= std::unique_ptr<T, nothrow_delete<T>>;

#endif // NEWOBJ_H__INCLUDED