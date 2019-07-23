#include "common.h"
// #include "ordered_array.h"
#include "kmalloc.h"
#include "new.h"
#include "PageT.h"
#include "PageDirectory.h"
#include "Frames.h"
#include "isr.h"
#include "vesavga.h"

extern "C" {
	extern uint64_t placement_address;
	//~ extern void set_page_directory(void *);
	extern void set_page64_directory(void *);
}

const uint64_t PAGE_SIZE = 0x1000;

using PTE = PageTableT<uint64_t,12>; // PageDirectory<PageTableT<uint64_t>, 12, 9>;
using PDE = PageDirectory<PTE, 21, 9>;
using PDPTE = PageDirectory<PDE, 30, 9>;
using PML4E = PageDirectory<PDPTE, 39, 9>;

using DIR32 = PageDirectory<PageTableT<uint32_t,10>, 10, 10>;
using DIR64 = PML4E; // PageDirectory<PDPTE, 39, 9>;


static DIR64 * getDIR64()
{
	uint32_t phys = 0;
	void *buffer = reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(DIR64), &phys));
	printf("DIR32: 0x%08.8x\n", (uint32_t)buffer);
	return new(buffer) DIR64{};
}


static DIR64 *current_directory64=nullptr;

extern "C"
{

	void initPaging64(uint64_t maxMem)
	{
		monitor_write("initPaging64()\n");
		auto *dir = getDIR64();

		printf("dir: 0x%08.8x, dir->physical 0x%08.8x, &dir 0x%08.8x, &(dir->physical): 0x%08.8x\n", (uint32_t)dir, (uint32_t)(dir->physical), (uint32_t)(&dir), (uint32_t)(&(dir->physical)) );
		printf("init Placement: 0x%08.8x\n", placement_address);
		auto *buffer = reinterpret_cast<void *>( kmalloc( sizeof(Frames<uint64_t>) ) );
		

		printf("Creating Frames: maxMem == %d\n", (uint32_t)maxMem );

		Frames<uint64_t> *frames64 = new(buffer) Frames<uint64_t>{maxMem};

		// new(reinterpret_cast<void *>(&frames32)) Frames<uint32_t>{maxMem};

		// We need to identity map (phys addr = virt addr) from
		// 0x0 to the end of used memory, so we can access this
		// transparently, as if paging wasn't enabled.
		// NOTE that we use a while loop here deliberately.
		// inside the loop body we actually change placement_address
		// by calling kmalloc(). A while loop causes this to be
		// computed on-the-fly rather than once at the start.

		int pages_mapped = 0;
		
		for( auto i=0u; i<maxMem || i<placement_address; i+= PAGE_SIZE)
		{
			// Kernel code is readable but not writeable from userspace.
			auto *page = dir->getPage(i);
			frames64->alloc(page, 1, 1);
			++pages_mapped;
		}
		
		
		printf("Mapped %d pages for %d bytes. final Placement: 0x%08.8x\n", pages_mapped, pages_mapped*4096, placement_address);
		
		printf("final Placement: 0x%08.8x\n", placement_address);
		// Now, enable paging!	

		current_directory64 = dir;
		set_page64_directory( dir );
	}
}


