#ifndef ORDERED_ARRAY_H_INCLUDED
#define ORDERED_ARRAY_H_INCLUDED

#include "common.h"
#include "kmalloc.h"

template< class T > struct remove_reference      {typedef T type;};
template< class T > struct remove_reference<T&>  {typedef T type;};
template< class T > struct remove_reference<T&&> {typedef T type;};

template<class _Ty>
	using remove_reference_t = typename remove_reference<_Ty>::type;

template <class T>
inline constexpr T&& forward(remove_reference_t<T> & t) noexcept
{
    return static_cast<T&&>(t);
}

template <class T>
inline constexpr T&& forward(remove_reference_t<T> && t) noexcept
{
    return static_cast<T&&>(t);
}

template< typename T>
inline constexpr typename remove_reference<T>::type&& move(T &&arg)noexcept
{
	return static_cast<remove_reference_t<T> &&>(arg);
}

template<typename T>
struct less
{
	u8int operator()(T a, T b)
	{
		return a<b ? 1 : 0;
	}
};

template<class T>
void swap( T &a, T &b)
{
	auto tmp = move(a);
	a = move(b);
	b = move(tmp);
}

template<typename T, typename Pred=less<T> >
struct ordered_array_t
{
	using value_type=T;
	
	value_type *start{0};
	value_type *last{0};
	value_type *end{0};
	Pred	pred{};
	
	ordered_array_t(value_type *ptr, u32int size)
		: start{ptr}
		, last{ptr}
		, end{ptr + size}
		, pred{}
		{
		}
	
	u32int size()const { return last - start; }
};

template<typename T, typename Pred=less<T> >
ordered_array_t<T, Pred> place_ordered_array(void *addr, u32int max_size)
{
	return ordered_array_t<T, Pred> { reinterpret_cast<T *>(addr), max_size };
	//~ ordered_array_t<T, Pred> to_ret;
	//~ const u32int alloc_size = max_size*sizeof(T);
	//~ to_ret.start = reinterpret_cast<T *>(addr);
	//~ memset(to_ret.array, 0, alloc_size);
	//~ to_ret.last = to_ret.start;
	//~ to_ret.end = to_ret.start + max_size;
	//~ to_ret.pred = move(lt);
}

template<typename T, typename Pred=less<T> >
ordered_array_t<T, Pred> create_ordered_array(u32int max_size)
{
	return place_ordered_array(kmalloc(max_size*sizeof(T)), max_size);
	//~ ordered_array_t<T, Pred> to_ret;
	//~ const u32int alloc_size = max_size*sizeof(T);
	//~ to_ret.start = reinterpret_cast<T *>(kmalloc(alloc_size));
	//~ memset(to_ret.array, 0, alloc_size);
	//~ to_ret.last = to_ret.start;
	//~ to_ret.end = to_ret.start + alloc_size;
	//~ return to_ret;
}

template<typename T, typename Pred=less<T> >
void destroy_ordered_array(ordered_array_t<T, Pred> *array)
{
// kfree(array->start);
}

template<typename T, typename Pred=less<T> >
void insert_ordered_array(type_t item, ordered_array_t<T, Pred> *array)
{
	T *it = 0;
	u32int size = array->size();
	for( it=array->start, it<array->last && array->pred(*it, item); ++iterator )
	{
	}
	if( it == array->last )
	{
		*it = item;
		++(array->last) ;
	}
	else 
	{
		swap(*it, item);
		while( it<array->last)
		{
			++it;
			auto tmp2 = move(*it);
			*it = move(item);
			item = move(tmp2);
		}
		++(array->last);
	}
}

template<typename T, typename Pred=less<T> >
T lookup_ordered_array(u32int i, ordered_array_t<T,Pred> *array)
{
	ASSERT(i < array->size());
	return array->start[i];
}

template<typename T, typename Pred=less<T> >
void remove_ordered_array(u32int i, ordered_array_t<T,Pred> *array)
{
	for( auto it = array->start + i, e=array->last; it<e; ++it)
	{
		*it = *(it+1);
	}
	--array->last;
}

#endif // ORDERED_ARRAY_H_INCLUDED
