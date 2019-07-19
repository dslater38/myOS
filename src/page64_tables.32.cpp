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
	extern uint32_t placement_address;
	extern void set_page_directory(void *);
	extern void set_page64_directory(void *);
	
	
}

const uint64_t PAGE_SIZE = 0x1000;

using PTE = PageTableT<uint64_t>; // PageDirectory<PageTableT<uint64_t>, 12, 9>;
using PDE = PageDirectory<PTE, 21, 9>;
using PDPTE = PageDirectory<PDE, 30, 9>;
using PML4E = PageDirectory<PDPTE, 39, 9>;

using DIR32 = PageDirectory<PageTableT<uint32_t>, 10, 10>;
using DIR64 = PageDirectory<PDPTE, 39, 9>;

static void printPageTables(uint32_t maxMem, uint64_t *PML4E);

static DIR32 *initDir32()
{
	uint32_t phys = 0;
	DIR32 *table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(DIR32), &phys))) DIR32{};
	table->setPhys(phys);
	return table;
}

static DIR32 * getDIR32()
{
	static DIR32 *table = initDir32();
	return table;
}

alignas(4096) DIR64 directory{};

static DIR64 *initDir64()
{
	uint32_t phys = 0;
	// DIR64 *table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(DIR64), &phys))) DIR64{};
	DIR64 *table = new((void *)&(directory)) DIR64{};
// 	table->setPhys(phys);
	return table;
}

static DIR64 * getDIR64()
{
	static DIR64 *table = initDir64();
	return table;
}

static Frames<uint32_t> *frames32 = nullptr;
static DIR32 *current_directory32=nullptr;

static Frames<uint64_t> *frames64 = nullptr;
static DIR64 *current_directory64=nullptr;

static void switch_page_directory32(DIR32 *dir)
{
	current_directory32 = dir;
	set_page_directory( dir);
}


extern "C"
{

void switch_page_directory64(DIR64 *dir)
{
	current_directory64 = dir;
	set_page64_directory( dir );
}

	
	void *get_directory32()
	{
		return getDIR32();
	}
	
	void *get_directory64()
	{
		return getDIR64();
	}
	
	void dumpDir(void *p)
	{
		reinterpret_cast<DIR32 *>(p)->dump();
	}
	
	void dumpDir64(void *p)
	{
		reinterpret_cast<DIR64 *>(p)->dump();
	}
	
	void initPaging32(uint32_t maxMem)
	{
		monitor_write32("initPaging32()\n");
		auto *dir = getDIR32();
		printf32("dir: 0x%08.8x, dir->physical 0x%08.8x, &dir 0x%08.8x, &(dir->physical): 0x%08.8x\n", (uint32_t)dir, (uint32_t)(dir->physical), (uint32_t)(&dir), (uint32_t)(&(dir->physical)) );
		
		
		printf32("init Placement: 0x%08.8x\n", placement_address);

		frames32 = new(reinterpret_cast<void *>(kmalloc(sizeof(Frames<uint32_t>)))) Frames<uint32_t>{maxMem};
		// new(reinterpret_cast<void *>(&frames32)) Frames<uint32_t>{maxMem};

		// We need to identity map (phys addr = virt addr) from
		// 0x0 to the end of used memory, so we can access this
		// transparently, as if paging wasn't enabled.
		// NOTE that we use a while loop here deliberately.
		// inside the loop body we actually change placement_address
		// by calling kmalloc(). A while loop causes this to be
		// computed on-the-fly rather than once at the start.

		int pages_mapped = 0;
		
		for( auto i=0u; i<placement_address; i+= PAGE_SIZE)
		{
			// Kernel code is readable but not writeable from userspace.
			auto *page = dir->getPage(i);
			frames32->alloc(page, 0, 0);
			++pages_mapped;
		}
				
		printf32("Mapped %d pages for %d bytes. final Placement: 0x%08.8x\n", pages_mapped, pages_mapped*4096, placement_address);
		
		// Now, enable paging!	
		switch_page_directory32(dir);
	}	
	
	
	void initPaging64(uint64_t maxMem)
	{
		monitor_write32("initPaging64()\n");
		auto *dir = getDIR64();

		printf32("dir: 0x%08.8x, dir->physical 0x%08.8x, &dir 0x%08.8x, &(dir->physical): 0x%08.8x\n", (uint32_t)dir, (uint32_t)(dir->physical), (uint32_t)(&dir), (uint32_t)(&(dir->physical)) );
		printf32("init Placement: 0x%08.8x\n", placement_address);
		auto *buffer = reinterpret_cast<void *>( kmalloc( sizeof(Frames<uint64_t>) ) );
		
		printf32("Creating Frames: maxMem == %d\n", (uint32_t)maxMem );
		
		frames64 = new(buffer) Frames<uint64_t>{maxMem};
		// new(reinterpret_cast<void *>(&frames32)) Frames<uint32_t>{maxMem};

		// We need to identity map (phys addr = virt addr) from
		// 0x0 to the end of used memory, so we can access this
		// transparently, as if paging wasn't enabled.
		// NOTE that we use a while loop here deliberately.
		// inside the loop body we actually change placement_address
		// by calling kmalloc(). A while loop causes this to be
		// computed on-the-fly rather than once at the start.

		int pages_mapped = 0;
		
		for( auto i=0u; i<placement_address; i+= PAGE_SIZE)
		{
			// Kernel code is readable but not writeable from userspace.
			auto *page = dir->getPage(i);
			frames64->alloc(page, 1, 1);
			++pages_mapped;
//			printf32(" i == 0x%08.8x, place == 0x%08.8x, gap == %d\n", i, placement_address, (placement_address - i));
		}
		
//		printPageTables((uint32_t)maxMem, dir->physical );
		
		printf32("Mapped %d pages for %d bytes. final Placement: 0x%08.8x\n", pages_mapped, pages_mapped*4096, placement_address);
		
		printf32("final Placement: 0x%08.8x\n", placement_address);
		// Now, enable paging!	
		switch_page_directory64(dir);
	}	

	
	void page_fault(registers_t regs)
	{
		// Output an error message.
		//monitor_write("Page fault! ( ");
		char buf[32];
		// A page fault has occurred.#include "common.h"

		// The faulting address is stored in the CR2 register.
		uint32_t faulting_address = get_fault_addr();
		// uint32_t faulting_address;
		// asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

		// The error code gives us details of what happened.
		int present   = !(regs.err_code & 0x1); // Page not present
		int rw = regs.err_code & 0x2;           // Write operation?
		int us = regs.err_code & 0x4;           // Processor was in user-mode?
		int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
		int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

		// Output an error message.
		monitor_write32("Page fault! ( ");
		if (present) {monitor_write32("present ");} else {monitor_write32("absent ");}
		if (rw) {monitor_write32("read-only ");} else {monitor_write32("read/write ");}
		if (us) {monitor_write32("user-mode ");} else {monitor_write32("kernel-mode ");}
		if (reserved) {monitor_write32("reserved ");} else {monitor_write32("not-reserved ");}
		sprintf32(buf, ") at 0x%08x", faulting_address);
		monitor_write32(buf);
		// monitor_write(") at 0x");
		// monitor_write_hex(faulting_address);
		monitor_write32("\n");
		PANIC("Page fault");
	}

}

static
inline
uint64_t *PTR(uint64_t n)
{
	return (uint64_t *)(((uint32_t)n) & 0xFFFFFFFC);
}

static
void printPageTables(uint32_t maxMem, uint64_t *pml4e)
{
	printf32("PageTable: maxMem: %d\n", maxMem);
	printf32("PML4E: %08.8x, PML4E[0]: %08.8x, PML4E[1]: %08.8x\n", (uint32_t)(pml4e), (uint32_t)(pml4e[0]), (uint32_t)(pml4e[1]) );
	
	ASSERT(pml4e[1] == 0);
	
	uint64_t *pdpte = PTR(pml4e[0]);

	ASSERT(pdpte != nullptr);

	printf32("\tPDPTE: %08.8x, PDPTE[0]: %08.8x, PDPTE[1]: %08.8x\n", (uint32_t)(pdpte), (uint32_t)(pdpte[0]), (uint32_t)(pdpte[1]) );
	
	uint64_t *pde = PTR(pdpte[0]);
	
	ASSERT(pde != nullptr);
	
	// printf("\t\tPDE: %08.8x, PDE[0]: %08.8x, PDE[1]: %08.8x\n", (uint32_t)(PDE), (uint32_t)(PDE[0]), (uint32_t)(PDE[1]) );
	
	for( auto i=0; i<512; ++i )
	{
		uint64_t *pte = PTR(pde[i]);
		if( pte != nullptr )
		{
			printf32("\t\tPTE[%d] %08.8x\n", i, (uint32_t)pte);
			for( auto j=0; j<512; ++j )
			{
				uint64_t uentry = pte[j];
				printf32("\t\t\tPageEntry[%d,%d] 0x%08.8x%08.8x\n", i, j, HIDWORD(uentry),LODWORD(uentry));
			}
			printf32("\t\t====================================\n");
		}
	}
	
	// try the lookups
	
	DIR64 *dir = (DIR64 *)pml4e;
	
	for( uint64_t i=0; i<maxMem; i+= 4096 )
	{
		const auto *page = dir->findPage(i);
		if( page )
		{
			uint64_t n = *((uint64_t *)(page));
			printf32("vaddr 0x%08.8x%08.8x ==> 0x%08.8x%08.8x\n", HIDWORD(i), LODWORD(i), HIDWORD(n), LODWORD(n));
		}
		else
		{
			printf32("vaddr 0x%08.8x%08.8x ==> <NULL>\n", HIDWORD(i), LODWORD(i));
		}
	}
}

