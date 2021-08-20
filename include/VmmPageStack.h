#ifndef VMMSTACK_H_INCLUDED
#define VMMSTACK_H_INCLUDED
#pragma once
#include <cstdint>
#include <cstddef>
#include "kmalloc.h"

using address_t = uint64_t;

namespace VM {
	class Manager;
};

struct PhysicalMemoryBlock
{
	// Note: These are frame numbers, not addresses
	// to convert to an address, multiply by pageSize
	uint64_t	start{0};				// Starting frame number for the block == physical_address / pageSize
	uint64_t	len{0};					// number of page sized frames in the block
	PhysicalMemoryBlock *next{nullptr};	// pointe to the next block of physical memory
	// The total number of frames for this struct, and all of its children.
	size_t totalCount()const
	{
		auto sz = count();
		if( next )
		{
			sz += next->totalCount();
		}
		return sz;
	}
	// The number of frames this struct represents.
	size_t count()const{
		return len;
	}
};

class VmmPageStack
{
	friend VM::Manager;
public:
	VmmPageStack() = default;
	explicit VmmPageStack(PhysicalMemoryBlock *physicalMemory_);

	address_t allocPage();
	void freePage(address_t addr);
	
	bool addBlock(PhysicalMemoryBlock *physicalMemory_);
	bool initStack(uint64_t &stackBaseVAddr, VM::Manager &mgr);
private:
	size_t size()const {
		return (stackEnd - stackBegin);
	}

	bool push(address_t addr);

	bool pop(address_t &addr);
	
	bool getNewFrame(address_t &addr);
	bool growStack();

	address_t	*stackBegin{0};
	address_t	*stackPtr {0};
	address_t	*stackEnd {0};
	address_t	*stackLast {0};
	size_t		pageSize{4096};
	PhysicalMemoryBlock	*physicalMemory{nullptr};
	VM::Manager *pMgr{nullptr};
};

#endif // VMMSTACK_H_INCLUDED
