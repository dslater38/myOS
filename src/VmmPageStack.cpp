#include <VmmPageStack.h>
#include <VMManager.h>
#include "common.h"

VmmPageStack::VmmPageStack(PhysicalMemoryBlock *physicalMemory_)
	:physicalMemory{physicalMemory_}
{
	if( physicalMemory_ )
	{
		const auto numPages = physicalMemory_->totalCount();
		const auto allocSize = sizeof(address_t) * numPages;
		stackBegin = reinterpret_cast<address_t *>(kmalloc_aligned(allocSize));
		if( !stackBegin )
		{
			PANIC("VmmStack::VmmStack() Failed to allocate memory for the stack.");
		}
		stackEnd = stackBegin + allocSize;
		stackPtr = stackBegin;
		push(allocPage());
	}
}

bool VmmPageStack::addBlock(PhysicalMemoryBlock *physicalMemory_)
{
	auto success = (physicalMemory_ != nullptr);
	if( success )
	{
		if( !physicalMemory )
		{
			physicalMemory = physicalMemory_;
		}
		else
		{
			while( physicalMemory->next != nullptr )
			{
				physicalMemory = physicalMemory->next;
			}
			physicalMemory->next = physicalMemory_;
		}
	}
	return success;
}

bool VmmPageStack::initStack(uint64_t &stackBaseVAddr, VM::Manager &mgr)
{
	pMgr = &mgr;
	auto success = (physicalMemory != nullptr);
	if( physicalMemory )
	{
		const auto numPages = physicalMemory->totalCount();
		const auto allocSize = sizeof(address_t) * numPages;
		stackBegin = reinterpret_cast<address_t *>(stackBaseVAddr);
		stackEnd = reinterpret_cast<address_t *>(reinterpret_cast <uint64_t >(stackBegin) + numPages*sizeof(address_t));
		stackPtr = stackLast = stackBegin;
		
//		mgr.mapPage(reinterpret_cast<uint64_t>(stackBegin), 0x03);
//		stackLast += pageSize/sizeof(address_t);
//		if( stackLast > stackEnd )
//		{
//			stackLast = stackEnd;
//		}
		
		stackBaseVAddr += allocSize;
		push(allocPage());
	}
	return success;
}

bool VmmPageStack::growStack()
{
	auto success = (stackLast < stackEnd);
	if( success && pMgr )
	{
		success = pMgr->mapPage(reinterpret_cast<uint64_t>(stackLast), 0x03);
		if( success )
		{
			stackLast += 512;
			success = true;
		}
	}
	return success;
}

address_t VmmPageStack::allocPage()
{
	address_t addr = 0;
	if( !pop(addr) )
	{
		PANIC("Out of physical memory.");
	}
	debug_out("allocated vAddr: 0x%08.8x%08.8x", HIDWORD(addr), LODWORD(addr));
	return addr;
}

void VmmPageStack::freePage(address_t addr)
{
	debug_out("free vAddr: 0x%08.8x%08.8x", HIDWORD(addr), LODWORD(addr));
	push(addr);
}

bool VmmPageStack::push(address_t addr)
{
	debug_out("push vAddr: 0x%08.8x%08.8x", HIDWORD(addr), LODWORD(addr));
	auto success = (stackPtr < stackLast);
	if( !success )
	{
		success = growStack();
	}
	if( success )
	{
		*stackPtr++ = addr;
	}
	return success;
}

bool VmmPageStack::pop(address_t &addr)
{
	auto success = (stackPtr > stackBegin);
	if( success )
	{
		addr = *(--stackPtr);
	}
	else
	{
		success = getNewFrame(addr);
	}
	debug_out("pop vAddr: 0x%08.8x%08.8x", HIDWORD(addr), LODWORD(addr));
	return success;
}

bool VmmPageStack::getNewFrame(address_t &addr)
{
	auto success = false;
	for( auto *ptr = physicalMemory; ptr != nullptr; ptr = ptr->next )
	{
		if( ptr->count() > 0 )
		{
			debug_out("alloc new frame: %lu\n", ptr->start);
			addr = ((ptr->start) * pageSize);
			ptr->start += 1;
			success = true;
			break;
		}
	}
	return success;
}
