#ifndef VM_MANAGER_H_INCLUDED
#define VM_MANAGER_H_INCLUDED
#pragma once

#include "multiboot2.h"
#include "VmmPageStack.h"
#include <memory>
#include "NewObj.h"
#include "PageT.h"
#include "kheap.h"
#include "PageDirectory.h"

namespace VM {

	class Manager
	{
	public:
		explicit Manager(const multiboot_tag_mmap *mmap);
		explicit Manager(PhysicalMemoryBlock *memList);
		bool mapPage(uint64_t vAddr, uint64_t flags);
		bool unMapPage(uint64_t vAddr);
		bool clearPage(uint64_t vAddr, uint64_t &frame);
		bool allocPages(uint64_t startAddress, size_t numPages, bool isKernel, bool isWritable);
		bool freePages(uint64_t startAddress, size_t numPages);
		heap_t	*getKernelHeap() { return kernelHeap; }
		static uint64_t getPhysicalAddress(uint64_t vAddr);
		static PML4E_4K	*getPML4_Table();
		static PDPTE_64_4K *getPDPT_Table(uint64_t vAddr);
		static PDE_64_4K *getPD_Table(uint64_t vAddr);
		static PTE_64_4K *getPT_Table(uint64_t vAddr);
	private:
		static Page4K *getPageEntry(uint64_t vAddr);
		bool	createHeap();
	private:
		VmmPageStack	frameStack{};
		VmmPageStack	lowMemFrameStack{};
		heap_t			*kernelHeap{nullptr};
	private:
		uint8_t			memory[sizeof(heap_t)];
	};
}
#endif // VM_MANAGER_H_INCLUDED
