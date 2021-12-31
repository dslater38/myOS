#include "VMManager.h"
#include "PageDirectory.h"

// constexpr uint64_t VM_BASE = 0x00000000C0000000;
constexpr uint64_t VM_BASE = 0xFFFF800000000000;

extern "C"
{
	extern void (*kernel_begin)();
	extern void (*kernel_end)();
	extern uint64_t placement_address;
	extern void invalidate_all_tlbs();
	extern void invalidate_tlb(uint64_t);
	
}

namespace VM {
	namespace {
		uint64_t *PML4EEntryVAddr(size_t index)
		{
			constexpr uint64_t recursiveAddr = 0xFFFFFFFFFFFFF000;
			if( index < 512 )
			{
				return reinterpret_cast<uint64_t *>(recursiveAddr) + index;
			}
			return nullptr;
		}

		uint64_t *PDPTEEntryVAddr(size_t plme4Index, size_t pdpteIndex)
		{
			const auto *plme4Entry = PML4EEntryVAddr(plme4Index);
			if( plme4Entry && (*plme4Entry & 0x01) && (pdpteIndex < 512) )
			{
				constexpr uint64_t recursiveAddr = 0xFFFFFFFFFFE00000;
				return reinterpret_cast<uint64_t *>(recursiveAddr | (plme4Index << 12)) + pdpteIndex;
			}
			return nullptr;
		}

		uint64_t *PDEEEntryVAddr(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex)
		{
			const auto *pdpteEntry = PDPTEEntryVAddr(plme4Index, pdpteIndex);
			if( pdpteEntry && (*pdpteEntry & 0x01) && pdeIndex < 512 )
			{
				constexpr uint64_t recursiveAddr = 0xFFFFFFFFC0000000;
				return reinterpret_cast<uint64_t *>( recursiveAddr | (plme4Index << 21) | (pdpteIndex << 12) ) + pdeIndex;
			}
			return nullptr;
		}

		uint64_t *PTEEntryVAddr(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex)
		{
			const auto *pdeEntry = PDEEEntryVAddr(plme4Index, pdpteIndex, pdeIndex);
			if( pdeEntry && (*pdeEntry & 0x01) && pteIndex < 512 )
			{
				constexpr uint64_t recursiveAddr = 0xFFFFFF8000000000; // 0xFFFFFF8000000000;
				return reinterpret_cast<uint64_t *>( recursiveAddr | (plme4Index << 30) | (pdpteIndex << 21) | (pdeIndex << 12) ) + pteIndex;
			}
			return nullptr;
		}

		uint64_t getPML4EEntry(size_t index)
		{
			const auto *vAddr = PML4EEntryVAddr(index);
			
			const auto *pDir = reinterpret_cast <PML4E_4K *>(PML4EEntryVAddr(0));
			auto entry = pDir ? pDir->physical[index] : 0;
			((void)entry);
			
			return vAddr ? *vAddr : 0u;
		}

		uint64_t getPDPTEEntry(size_t plme4Index, size_t pdpteIndex)
		{
			const auto *vAddr = PDPTEEntryVAddr(plme4Index, pdpteIndex);
			
			const auto *pDir = reinterpret_cast <PML4E_4K *>(PDPTEEntryVAddr(plme4Index, 0));
			auto entry = pDir ? pDir->physical[pdpteIndex] : 0;
			((void)entry);
			
			
			return vAddr ? *vAddr : 0u;
		}

		uint64_t getPDEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex)
		{
			const auto *vAddr = PDEEEntryVAddr(plme4Index, pdpteIndex, pdeIndex);
			
			
			const auto *pDir = reinterpret_cast <PDPTE_64_4K *>(PDEEEntryVAddr(plme4Index, pdpteIndex, 0));
			auto entry = pDir ? pDir->physical[pdeIndex] : 0;
			((void)entry);
			
			
			return vAddr ? *vAddr : 0u;
		}

		uint64_t getPTEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex)
		{
			const auto *vAddr = PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, pteIndex);
			
			const auto *pDir = reinterpret_cast <PDE_64_4K *>(PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, 0));
			auto entry = pDir ? pDir->physical[pteIndex] : 0;
			
			return vAddr ? *vAddr : 0u;
		}

		bool setPML4EEntry(size_t index, uint64_t val)
		{
			auto *pDir = reinterpret_cast<PML4E_4K *>(PML4EEntryVAddr(0));
			if( pDir && val < 512 )
			{
				pDir->physical[index] = val;
				invalidate_all_tlbs();
				auto *ptr = PML4EEntryVAddr(index);
				memset(ptr, 0, 4096);
				return true;
			}
			return false;
		}

		bool setPDPTEEntry(size_t plme4Index, size_t pdpteIndex, uint64_t val)
		{
			auto *pDir = reinterpret_cast<PDPTE_64_4K *>(PDPTEEntryVAddr(plme4Index, 0));
			if( pDir )
			{
				pDir->physical[pdpteIndex] = val;
				invalidate_all_tlbs();	
				auto *ptr = PDPTEEntryVAddr(plme4Index, pdpteIndex);
				memset(ptr, 0, 4096);
				
				return true;
			}
			return false;
		}

		bool setPDEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, uint64_t val)
		{
			auto *pDir = reinterpret_cast <PDE_64_4K *>(PDEEEntryVAddr(plme4Index, pdpteIndex, 0));
			if( pDir )
			{
				pDir->physical[pdeIndex] = val;
				invalidate_all_tlbs();
				auto foo = PDEEEntryVAddr(plme4Index, pdpteIndex, pdeIndex);
				((void)foo);
				auto *ptr = reinterpret_cast <PTE_64_4K *>(PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, 0));
				memset(ptr->physical, 0, 4096);
				
				return true;
			}
			return false;
		}

		bool setPTEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex, uint64_t val)
		{
			auto *pDir = reinterpret_cast <PTE_64_4K *>(PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, 0));
			if( pDir )
			{
				pDir->physical[pteIndex] = val;
				invalidate_all_tlbs();
				// can only write the page if it is present and writable.
				if( (val & 0x03) == 0x03 )
				{
					auto ptr = reinterpret_cast<uint64_t *>((plme4Index << 39) | (pdpteIndex << 30) | (pdeIndex << 21) | (pteIndex << 12));
					memset(ptr, 0, 4096);
				}
				return true;
			}
			return false;
		}

		bool getPTEFrame(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex, uint64_t &val)
		{
			auto *pDir = reinterpret_cast <PTE_64_4K *>(PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, 0));
			if( pDir )
			{
				val = ( pDir->physical[pteIndex] & 0xFFFFFFFFFFFFF000 );
				return true;
			}
			return false;
		}

		uint64_t *lookupPTEEntry(uint64_t vAddr)
		{
			size_t plme4Index = ((vAddr >> 39) & 0x00000000000001FFu);
			size_t pdpteIndex = ((vAddr >> 30) & 0x00000000000001FFu);
			size_t pdeIndex = ((vAddr >> 21) & 0x00000000000001FFu);
			size_t pteIndex = ((vAddr >> 12) & 0x00000000000001FFu);
			return PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, pteIndex);
		}
		
		PDPTE_64_4K *getPlme4Entry(uint64_t vAddr)
		{
			const auto index = PML4E_4K::index(vAddr);
			auto entry = getPML4EEntry(index);
			return reinterpret_cast<PDPTE_64_4K *>(entry & 0xFFFFFFFFFFFF000);
		}
		
		PDE_64_4K *getPdpteEntry(uint64_t vAddr)
		{
			const auto index1 = PML4E_4K::index(vAddr);
			const auto index2 = PDPTE_64_4K::index(vAddr);
			return reinterpret_cast<PDE_64_4K *>(getPDPTEEntry(index1, index2) & 0xFFFFFFFFFFFF000);
		}		
		
		PTE_64_4K *getPdeEntry(uint64_t vAddr)
		{
			const auto index1 = PML4E_4K::index(vAddr);
			const auto index2 = PDPTE_64_4K::index(vAddr);
			const auto index3 = PDE_64_4K::index(vAddr);
			return reinterpret_cast<PTE_64_4K *>(getPDEEntry(index1, index2, index3) & 0xFFFFFFFFFFFF000);
		}		
		
		Page4K *getPteEntry(uint64_t vAddr)
		{
			const auto index1 = PML4E_4K::index(vAddr);
			const auto index2 = PDPTE_64_4K::index(vAddr);
			const auto index3 = PDE_64_4K::index(vAddr);
			const auto index4 = PTE_64_4K::index(vAddr);
			return reinterpret_cast<Page4K *>(getPTEEntry(index1, index2, index3, index4) & 0xFFFFFFFFFFFF000);
		}		
	}
	
	Manager::Manager(PhysicalMemoryBlock *memList)
	{
		while(memList)
		{
			auto *ptr = memList;
			memList = memList->next;
			ptr->next = nullptr;
			VmmPageStack  &stack = (ptr->start <= 0x0000000000000100) ? lowMemFrameStack : frameStack;
			stack.addBlock(ptr);
		}
	}
	
	Manager::Manager(const multiboot_tag_mmap *mmapStart)
	{
		for( auto *mmap = mmapStart->entries;
				reinterpret_cast<const multiboot_uint8_t *>(mmap) < reinterpret_cast<const multiboot_uint8_t *>(mmapStart) + mmapStart->size;
				mmap = reinterpret_cast<const multiboot_memory_map_t *> (reinterpret_cast<const uint8_t *>(mmap) + mmapStart->entry_size) )
		{
			if( mmap->type == MULTIBOOT_MEMORY_AVAILABLE )
			{
				auto *newBlock = New<PhysicalMemoryBlock>();
				newBlock->start = ((mmap->addr) >> 12);
				newBlock->len = ((mmap->len) >> 12);
				VmmPageStack  &stack = (mmap->addr <= 0x0000000000000100) ? lowMemFrameStack : frameStack;
				stack.addBlock(newBlock);
			}
		}
		//frameStack.initStack();
		//lowMemFrameStack.initStack();
		

		const uint64_t kernelStartFrame = (VM::Manager::getPhysicalAddress(reinterpret_cast<uint64_t>(&kernel_begin)) >> 12);
		// uint64_t kernelEndFrame = (0x0000000000200000>>12); // (( (placement_address+4095) - VM_BASE) >> 12);
// 		uint64_t kernelEndFrame = (( (placement_address+4095) - VM_BASE) >> 12);
		const uint64_t kernelEndFrame = (VM::Manager::getPhysicalAddress(placement_address+4095)>>12);
		auto kernelLen = kernelEndFrame - kernelStartFrame;
		// now, we need to trim the kernel block out of the PhysicalMemoryBlock blocks so we don't allocate
		// kernel space 
		for( auto *ptr = frameStack.physicalMemory; ptr != nullptr;ptr=ptr->next)
		{
			const auto pEnd = ptr->start + ptr->len;
			
			if( ptr->start <= kernelStartFrame )
			{
				if( pEnd >= kernelStartFrame )
				{
					if( pEnd <= kernelEndFrame )
					{
						// kernel starts after start and extends to the end of this block
						// just need to trim the length
						const auto newLen = (kernelStartFrame - ptr->start);
						ptr->len = newLen;
					}
					else
					{
						// the kernel startsafter ptr->start, and ends before ptr->start + ptr->len
						// need to split the block
						auto newLen = (kernelStartFrame - ptr->start);
						if( newLen == 0 )
						{
							// the kernel starts at the beginning on the block
							// just need to update ptr->start
							newLen = (ptr->start + ptr->len) - kernelEndFrame;
							ptr->start = kernelEndFrame;
							ptr->len = newLen;
						}
						else
						{
							// have space before and after the kernel
							// need to split the block
							auto newLen = (kernelStartFrame - ptr->start);
							const auto oldLen = ptr->len;
							ptr->len = newLen;
							const auto newStart = kernelEndFrame;
							newLen = (ptr->start + oldLen) - newStart;
							auto *newBlock = New<PhysicalMemoryBlock>();
							newBlock->start = newStart;
							newBlock->len = newLen;
							newBlock->next = ptr->next;
							ptr->next = newBlock;							
						}
					}
				}
			}
		}
		uint64_t stackBaseVAddr = 0x00000000C0200000;
#if 0		
		auto stackBaseVAddr = ((placement_address+4095) & 0xFFFFFFFFFFFFF000);	// the page stacks virtual addresses start at the next page boundary after placement_address
		// At this point, we still have mappings in our page table from placement_address up to 2MB
		// Lets unmap them, as the physical addresses are part of our physicalMemory block.
		auto *pDir = getPdeEntry(placement_address);
		if( pDir )
		{
			auto *ptr = pDir->physical;
			for( uint64_t index = PTE_64_4K::index(stackBaseVAddr); index < 512; ++index )
			{
				ptr[index] = 0;
			}
			invalidate_all_tlbs();
		}
#endif
		frameStack.initStack(stackBaseVAddr, *this);
		stackBaseVAddr = ((stackBaseVAddr + 4095) & 0xFFFFFFFFFFFFF000);
		const auto nPages = (((KHEAP_INITIAL_SIZE + 4095) & 0xFFFFFFFFFFFFF000))>>12;
		if( allocPages(stackBaseVAddr, nPages, true, true ) )
		{
			kernelHeap = new(reinterpret_cast<void *>(memory)) heap_t{this, stackBaseVAddr, stackBaseVAddr + KHEAP_INITIAL_SIZE, HEAK_MAX_SIZE, 0, 0};
		}
	}
	
	
	Page4K *Manager::getPageEntry(uint64_t vAddr)
	{
		return nullptr;
	}
	
	bool Manager::clearPage(uint64_t vAddr, uint64_t &frame)
	{
		auto success = false;
		auto *pmle4Entry = getPlme4Entry(vAddr);
		if( pmle4Entry )
		{
			auto *pdptEntry = getPdpteEntry(vAddr);
			if( pdptEntry )
			{
				auto *pdeEntry = getPdeEntry(vAddr);
				if( pdeEntry )
				{
					auto *pteEntry = getPteEntry(vAddr);
					if(pteEntry)
					{
						uint64_t tmp = 0;
						auto success = getPTEFrame(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), PDE_64_4K::index(vAddr), PTE_64_4K::index(vAddr), tmp);
						if( success )
						{
							setPTEEntry(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), PDE_64_4K::index(vAddr), PTE_64_4K::index(vAddr), 0x00 | 0x00);
							frame = tmp;
							return true;
						}
					}
				}
			}			
		}
		return false;
	}

	bool Manager::unMapPage(uint64_t vAddr)
	{
		uint64_t frame = 0;
		auto success = clearPage(vAddr, frame);
		if(success)
		{
			if( frame < 1024 * 1024 )
			{
				lowMemFrameStack.freePage(frame);
			}
			else
			{
				frameStack.freePage(frame);
			}
			invalidate_tlb(vAddr);
		}
		return success;
#if 0		
		auto success = false;
		auto *pmle4Entry = getPlme4Entry(vAddr);
		if( pmle4Entry )
		{
			auto *pdptEntry = getPdpteEntry(vAddr);
			if( pdptEntry )
			{
				auto *pdeEntry = getPdeEntry(vAddr);
				if( pdeEntry )
				{
					auto *pteEntry = getPteEntry(vAddr);
					if(pteEntry)
					{
						uint64_t frame = 0;
						auto success = getPTEFrame(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), PDE_64_4K::index(vAddr), PTE_64_4K::index(vAddr), frame);
						if( success )
						{
							setPTEEntry(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), PDE_64_4K::index(vAddr), PTE_64_4K::index(vAddr), 0x00 | 0x00);
							if( frame < 1024 * 1024 )
							{
								lowMemFrameStack.freePage(frame);
							}
							else
							{
								frameStack.freePage(frame);
							}
							invalidate_tlb(vAddr);
							return true;
						}
					}
				}
			}			
		}
		return false;
#endif // 0		
	}
	
	bool Manager::mapPage(uint64_t vAddr, uint64_t flags)
	{
		auto success = false;
		auto *pmle4Entry = getPlme4Entry(vAddr);
		if( !pmle4Entry )
		{
			auto newFr = frameStack.allocPage();
			setPML4EEntry(PML4E_4K::index(vAddr), newFr | 0x03);
			pmle4Entry = getPlme4Entry(vAddr);
		}
		if( pmle4Entry )
		{			
			auto *pdptEntry = getPdpteEntry(vAddr);
			if( !pdptEntry )
			{
				auto newFr = frameStack.allocPage();
				setPDPTEEntry(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), newFr | 0x03);
				pdptEntry = getPdpteEntry(vAddr);
			}
			if( pdptEntry )
			{
				auto *pdeEntry = getPdeEntry(vAddr);
				if( !pdeEntry )
				{
					auto newFr = frameStack.allocPage();
					setPDEEntry(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), PDE_64_4K::index(vAddr), newFr | 0x03);
					pdeEntry = getPdeEntry(vAddr);					
				}
				if( pdeEntry )
				{

					auto *pteEntry = getPteEntry(vAddr);
					if(!pteEntry)
					{
						auto newFr = frameStack.allocPage();						
						setPTEEntry(PML4E_4K::index(vAddr), PDPTE_64_4K::index(vAddr), PDE_64_4K::index(vAddr), PTE_64_4K::index(vAddr), newFr | 0x03);
						pteEntry = getPteEntry(vAddr);
					}
					invalidate_tlb(vAddr);
					memset(reinterpret_cast <uint64_t *>(vAddr), 0, 4096);
					success = true;
				}
			}
		}
		
		return success;
	}

	uint64_t Manager::getPhysicalAddress(uint64_t vAddr)
	{
		uint64_t frame = 0;
		auto *pmle4Entry = getPlme4Entry(vAddr);
		if( pmle4Entry )
		{			
			auto *pdptEntry = getPdpteEntry(vAddr);
			if( pdptEntry )
			{
				auto *pdeEntry = getPdeEntry(vAddr);
				if( pdeEntry )
				{
					const auto index1 = PML4E_4K::index(vAddr);
					const auto index2 = PDPTE_64_4K::index(vAddr);
					const auto index3 = PDE_64_4K::index(vAddr);
					const auto index4 = PTE_64_4K::index(vAddr);
					uint64_t val = 0;
					if( getPTEFrame(index1, index2, index3, index4, val) )
					{
						frame = val;
					}
				}
			}
		}
		return frame;
	}
	
	bool Manager::allocPages(uint64_t startAddress, size_t numPages, bool isKernel, bool isWritable)
	{
		for( auto i = 0u; i < numPages; ++i )
		{
			uint64_t flags = PageBits::Present;
			if( isWritable )
			{
				flags |= PageBits::ReadWrite;
			}
			if( !isKernel )
			{
				flags |= PageBits::User;
			}
			if( !mapPage(startAddress, flags) )
			{
				return false;
			}
			startAddress += 4096;
		}
		return true;
	}
	
	bool Manager::freePages(uint64_t startAddress, size_t numPages)
	{
		for( auto i = 0u; i < numPages; ++i )
		{
			if( !unMapPage(startAddress) )
			{
				return false;
			}
			startAddress += 4096;
		}
		return true;
	}
	
}
