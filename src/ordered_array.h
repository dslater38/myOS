#ifndef ORDERED_ARRAY_H_INCLUDED
#define ORDERED_ARRAY_H_INCLUDED

#include "common.h"
#include "kmalloc.h"
#include <type_traits>
#include <functional>
#include <utility>


// template<typename T>
// struct less
// {
// 	bool operator()(T a, T b)
// 	{
// 		return (a<b);
// 	}
// };

// template<class T>
// void swap( T &a, T &b)
// {
// 	auto tmp = move(a);
// 	a = move(b);
// 	b = move(tmp);
// }

template<typename T, typename Pred=std::less<T> >
struct ordered_array_t
{
	using value_type=T;
	
	value_type *start{0};
	value_type *last{0};
	value_type *end{0};
	Pred	pred{};
	
	ordered_array_t(value_type *ptr, uint64_t size)
		: start{ptr}
		, last{ptr}
		, end{ptr + size}
		, pred{}
		{
		}
	
	uint64_t size()const { return last - start; }
};

template<typename T, typename Pred=std::less<T> >
ordered_array_t<T, Pred> place_ordered_array(void *addr, uint64_t max_size)
{
	return ordered_array_t<T, Pred> { reinterpret_cast<T *>(addr), max_size };
	//~ ordered_array_t<T, Pred> to_ret;
	//~ const uint64_t alloc_size = max_size*sizeof(T);
	//~ to_ret.start = reinterpret_cast<T *>(addr);
	//~ memset(to_ret.array, 0, alloc_size);
	//~ to_ret.last = to_ret.start;
	//~ to_ret.end = to_ret.start + max_size;
	//~ to_ret.pred = move(lt);
}

template<typename T, typename Pred=std::less<T> >
ordered_array_t<T, Pred> create_ordered_array(uint64_t max_size)
{
	return place_ordered_array<T,Pred>(kmalloc(max_size*sizeof(T)), max_size);
	//~ ordered_array_t<T, Pred> to_ret;
	//~ const uint64_t alloc_size = max_size*sizeof(T);
	//~ to_ret.start = reinterpret_cast<T *>(kmalloc(alloc_size));
	//~ memset(to_ret.array, 0, alloc_size);
	//~ to_ret.last = to_ret.start;
	//~ to_ret.end = to_ret.start + alloc_size;
	//~ return to_ret;
}

template<typename T, typename Pred=std::less<T> >
void destroy_ordered_array(ordered_array_t<T, Pred> *array)
{
// kfree(array->start);
}

template<typename T, typename Pred=std::less<T> >
void insert_ordered_array(T item, ordered_array_t<T, Pred> *array)
{
	T *it = 0;
	auto size = array->size();
	for( it=array->start; it<array->last && array->pred(*it, item); ++it )
	{
	}
	if( it == array->last )
	{
		*it = item;
		++(array->last) ;
	}
	else 
	{
		std::swap(*it, item);
		while( it<array->last)
		{
			++it;
			std::swap(*it, item);
			// auto tmp2 = std::move(*it);
			// *it = std::move(item);
			// item = std::move(tmp2);
		}
		++(array->last);
	}
}

template<typename T, typename Pred=std::less<T> >
T lookup_ordered_array(uint64_t i, ordered_array_t<T,Pred> *array)
{
	ASSERT(i < array->size());
	return array->start[i];
}

template<typename T, typename Pred=std::less<T> >
void remove_ordered_array(uint64_t i, ordered_array_t<T,Pred> *array)
{
	for( auto it = array->start + i, e=array->last; it<e; ++it)
	{
		*it = *(it+1);
	}
	--array->last;
}

#endif // ORDERED_ARRAY_H_INCLUDED
