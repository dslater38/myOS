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
}

const u64int PAGE_SIZE = 0x1000;
using page32_t=PageT<u32int>;
// using page_table32_t=PageTableT<u32int>;
// using page64_t=PageT<u64int>;
using page_table64_t=PageTableT<u64int>;

using PTE = PageDirectory<page_table64_t, 12, 9>;
using PDE = PageDirectory<PTE, 21, 9>;
using PDPTE = PageDirectory<PDE, 30, 9>;
using PML4E = PageDirectory<PDPTE, 39, 9>;

using DIR32 = PageDirectory<PageTableT<u32int>, 10, 10>;

static
u64int
pml4eTable()
{
	static PML4E table{};
	return reinterpret_cast<u64int>(&table);
}

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

static PML4E * make_page_tables(u64int ramSize)
{
	u64int uPml4e = pml4eTable();
	PML4E * pml4e= reinterpret_cast<PML4E *>(uPml4e);
	// auto numPages = ramSize>>12;
	for( auto addr = 0; addr < ramSize; addr += PAGE_SIZE )
	{
		pml4e->getPage(addr);
	}
	return pml4e;
}


static Frames<u32int> *frames32 = nullptr;
static DIR32 *current_directory32=nullptr;

static void switch_page_directory32(DIR32 *dir)
{
    //~ current_directory = dir;
    //~ asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    //~ u32int cr0;
    //~ asm volatile("mov %%cr0, %0": "=r"(cr0));
    //~ cr0 |= 0x80000000; // Enable paging!
    //~ asm volatile("mov %0, %%cr0":: "r"(cr0));
	
	//~ monitor_write("dir == ");
	//~ monitor_write_hex((u32int)dir);
	//~ monitor_write("\n");
	
	// printf("switch: dir 0x%08.8x, phys: 0x%08.8x, &phys: 0x%08.8x\n",(u32int)dir, (u32int)(dir->physicalAddr), (u32int)(&(dir->physicalAddr)));
	
	current_directory32 = dir;
	
	//~ monitor_write("&phys == ");
	//~ monitor_write_hex((u32int)&(dir->tablesPhysical));
	//~ monitor_write("\n");

	//~ monitor_write("phys == ");
	//~ monitor_write_hex((u32int)(dir->tablesPhysical));
	//~ monitor_write("\n");
	
	
	set_page_directory( dir->physical );

}


extern "C"
{

	// void switch_page_directory(void *dir);
	
	void *get_directory32()
	{
		return getDIR32();
	}
	
	page32_t *get_page32(u32int address)
	{
		return getDIR32()->getPage(address);
	}
	
	//~ void *init_paging()
	//~ {	
		//~ __asm__ volatile ( "movl %eax, %edx\n movb $0x39, %al\n outb %al, $0xe9 \n movl %edx,%eax");
		//~ void *p = make_page_tables(33554432);
		//~ __asm__ volatile ( "movl %eax, %edx\n movb $0x39, %al\n outb %al, $0xe9 \n movl %edx,%eax");
		//~ return p;
	//~ }
	
	void dumpDir(void *p)
	{
		reinterpret_cast<DIR32 *>(p)->dump();
	}
	
	void initPaging32(u32int maxMem)
	{
	//~ monitor_write("placement_address: ");
	//~ monitor_write_dec(placement_address);
	//~ monitor_write("\n\n");
		
		auto *dir = getDIR32();
		
	//~ monitor_write("dir: ");
	//~ monitor_write_hex((u32int)dir);
	//~ monitor_write(" phy: ");
	//~ monitor_write_hex((u32int)(dir->physicalAddr));
	//~ monitor_write(" &phy: ");
	//~ monitor_write_hex((u32int)(&(dir->physicalAddr)));		
	//~ monitor_write(" [0]: ");
	//~ monitor_write_hex((u32int)(dir->tables[0]));
	//~ monitor_write("\n");
	//~ monitor_write("page 0: ");
	PageTableT<u32int> *ptr = reinterpret_cast<PageTableT<u32int> *>(dir->tables[0]);
	//~ monitor_write_hex( (*ptr)[0] );
		//~ monitor_write("\n");
		
		
		
		//~ monitor_write("dir : ");
		//~ monitor_write_hex((u32int)dir);
		//~ monitor_write(" phys: ");
		//~ monitor_write_hex(dir->physicalAddr);
		//~ monitor_write("\n");
		frames32 = new(reinterpret_cast<void *>(kmalloc(sizeof(Frames<u32int>)))) Frames<u32int>{maxMem};
		// new(reinterpret_cast<void *>(&frames32)) Frames<u32int>{maxMem};
		
		// The size of physical memory. For the moment we
		// assume it is 16MB big.
		// u32int mem_end_page = maxMem;

		// We need to identity map (phys addr = virt addr) from
		// 0x0 to the end of used memory, so we can access this
		// transparently, as if paging wasn't enabled.
		// NOTE that we use a while loop here deliberately.
		// inside the loop body we actually change placement_address
		// by calling kmalloc(). A while loop causes this to be
		// computed on-the-fly rather than once at the start.
		int i = 0;
		while (i < placement_address)
		// int i;
		// for( i=0; i<0x1000000; i += PAGE_SIZE )
		{
			// Kernel code is readable but not writeable from userspace.
			//monitor_write("         i: ");
			//monitor_write_dec(i);
			//monitor_write("\n");
			// alloc_frame( get_page32(i), 0, 0);
			auto *page = dir->getPage(i);
			
			frames32->alloc(page, 0, 0);
			i += PAGE_SIZE;
			
		//~ monitor_write("Page: ");
		//~ monitor_write_hex((u32int)page);
		//~ monitor_write(" frame: ");
		//~ monitor_write_hex(page->frame);
		//~ monitor_write("\n");
			
		}
		// Now, enable paging!
		//monitor_write("Before switch_page_directory\n");
		
	//~ monitor_write("dir: ");
	//~ monitor_write_hex((u32int)dir);
	//~ monitor_write(" phy: ");
	//~ monitor_write_hex((u32int)(dir->physicalAddr));
	//~ monitor_write(" &phy: ");
	//~ monitor_write_hex((u32int)(&(dir->physicalAddr)));				
	//~ monitor_write(" [0]: ");
	//~ monitor_write_hex((u32int)(dir->tables[0]));
	//~ monitor_write("\n");
	//~ monitor_write("page 0: ");
	PageTableT<u32int> * table = reinterpret_cast<PageTableT<u32int> *>(dir->tables[0]);
	u32int entry = (*table)[0];
	//~ monitor_write_hex( entry );
	//~ monitor_write("\n");
	// monitor_write_hex( (u32int)(reinterpret_cast<PageTableT<u32int> *>(dir->tables[0])->getPage(0) ));
//	monitor_write("\nDumping...\n");
		
	//	dir->dump();
		
		switch_page_directory32(dir);
		//monitor_write("After switch_page_directory\n");
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
