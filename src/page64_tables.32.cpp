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
	extern u32int placement_address;
	extern void set_page_directory(void *);
	extern void set_page64_directory(void *);
}

const u64int PAGE_SIZE = 0x1000;

using PTE = PageDirectory<PageTableT<u64int>, 12, 9>;
using PDE = PageDirectory<PTE, 21, 9>;
using PDPTE = PageDirectory<PDE, 30, 9>;
using PML4E = PageDirectory<PDPTE, 39, 9>;

using DIR32 = PageDirectory<PageTableT<u32int>, 10, 10>;
using DIR64 = PageDirectory<PDPTE, 39, 9>;

static DIR32 *initDir32()
{
	u32int phys = 0;
	DIR32 *table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(DIR32), &phys))) DIR32{};
	table->setPhys(phys);
	return table;
}

static DIR32 * getDIR32()
{
	static DIR32 *table = initDir32();
	return table;
}

static DIR64 *initDir64()
{
	u32int phys = 0;
	DIR64 *table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(DIR64), &phys))) DIR64{};
	table->setPhys(phys);
	return table;
}

static DIR64 * getDIR64()
{
	static DIR64 *table = initDir64();
	return table;
}

static Frames<u32int> *frames32 = nullptr;
static DIR32 *current_directory32=nullptr;

static Frames<u64int> *frames64 = nullptr;
static DIR64 *current_directory64=nullptr;

static void switch_page_directory32(DIR32 *dir)
{
	current_directory32 = dir;
	set_page_directory( dir);
}


static void switch_page_directory64(DIR64 *dir)
{
	current_directory64 = dir;
	set_page64_directory( dir );
}

extern "C"
{
	
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
	
	void initPaging32(u32int maxMem)
	{
		monitor_write("initPaging32()\n");
		auto *dir = getDIR32();
		PageTableT<u32int> *ptr = reinterpret_cast<PageTableT<u32int> *>(dir->tables[0]);

		frames32 = new(reinterpret_cast<void *>(kmalloc(sizeof(Frames<u32int>)))) Frames<u32int>{maxMem};
		// new(reinterpret_cast<void *>(&frames32)) Frames<u32int>{maxMem};

		// We need to identity map (phys addr = virt addr) from
		// 0x0 to the end of used memory, so we can access this
		// transparently, as if paging wasn't enabled.
		// NOTE that we use a while loop here deliberately.
		// inside the loop body we actually change placement_address
		// by calling kmalloc(). A while loop causes this to be
		// computed on-the-fly rather than once at the start.

		for( auto i=0u; i<placement_address; i+= PAGE_SIZE)
		{
			// Kernel code is readable but not writeable from userspace.
			auto *page = dir->getPage(i);
			frames32->alloc(page, 0, 0);
		}
		// Now, enable paging!	
		switch_page_directory32(dir);
	}	
	
	
	void initPaging64(u64int maxMem)
	{
		monitor_write("initPaging64()\n");
		auto *dir = getDIR64();
		PageTableT<u64int> *ptr = reinterpret_cast<PageTableT<u64int> *>(dir->tables[0]);

		frames64 = new(reinterpret_cast<void *>(kmalloc(sizeof(Frames<u64int>)))) Frames<u64int>{maxMem};
		// new(reinterpret_cast<void *>(&frames32)) Frames<u32int>{maxMem};

		// We need to identity map (phys addr = virt addr) from
		// 0x0 to the end of used memory, so we can access this
		// transparently, as if paging wasn't enabled.
		// NOTE that we use a while loop here deliberately.
		// inside the loop body we actually change placement_address
		// by calling kmalloc(). A while loop causes this to be
		// computed on-the-fly rather than once at the start.

		for( auto i=0u; i<placement_address; i+= PAGE_SIZE)
		{
			// Kernel code is readable but not writeable from userspace.
			auto *page = dir->getPage(i);
			frames64->alloc(page, 0, 0);
		}
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
		u32int faulting_address = get_fault_addr();
		// u32int faulting_address;
		// asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

		// The error code gives us details of what happened.
		int present   = !(regs.err_code & 0x1); // Page not present
		int rw = regs.err_code & 0x2;           // Write operation?
		int us = regs.err_code & 0x4;           // Processor was in user-mode?
		int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
		int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

		// Output an error message.
		monitor_write("Page fault! ( ");
		if (present) {monitor_write("present ");} else {monitor_write("absent ");}
		if (rw) {monitor_write("read-only ");} else {monitor_write("read/write ");}
		if (us) {monitor_write("user-mode ");} else {monitor_write("kernel-mode ");}
		if (reserved) {monitor_write("reserved ");} else {monitor_write("not-reserved ");}
		sprintf(buf, ") at 0x%08x", faulting_address);
		monitor_write(buf);
		// monitor_write(") at 0x");
		// monitor_write_hex(faulting_address);
		monitor_write("\n");
		PANIC("Page fault");
	}

}
