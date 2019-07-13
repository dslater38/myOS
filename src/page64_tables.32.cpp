#include "common.h"
// #include "ordered_array.h"
#include "kmalloc.h"
#include "new.h"


const u64int PAGE_SIZE = 0x1000;


typedef struct page32
{
	u32int present    : 1;   // Page present in memory
	u32int rw         : 1;   // Read-only if clear, readwrite if set
	u32int user       : 1;   // Supervisor level only if clear
	u32int accessed   : 1;   // Has the page been accessed since last refresh?
	u32int dirty      : 1;   // Has the page been written to since last refresh?
	u32int unused     : 7;   // Amalgamation of unused and reserved bits
	u32int frame      : 20;  // Frame address (shifted right 12 bits)
}page32_t;

typedef struct page_table32
{
	using PageType=page32_t;
	page32_t	pages[4096];
	
	page32_t *getPage(u32int vaddr)
	{
		return &(pages[vaddr & 0x0FFF]);
	}
	
	void setPhys(u32int){}
	
}page_table32_t;

typedef struct page64
{
	u64int present    : 1;   // Page present in memory
	u64int rw         : 1;   // Read-only if clear, readwrite if set
	u64int user       : 1;   // Supervisor level only if clear
	u64int accessed   : 1;   // Has the page been accessed since last refresh?
	u64int dirty      : 1;   // Has the page been written to since last refresh?
	u64int unused     : 7;   // Amalgamation of unused and reserved bits
	u64int frame      : 52;  // Frame address (shifted right 12 bits)
}page64_t;


typedef struct page_table64
{
	using PageType=page64_t;
	page64_t	pages[4096];
	
	page64_t *getPage(u64int vaddr)
	{
		return &(pages[vaddr & 0x0FFF]);
	}
	
	void setPhys(u64int){}
	
}page_table64_t;

template<typename T, typename Pointer, const Pointer SHIFT>
struct directory
{
	using PageType=typename T::PageType;
	
	directory()
	{
		memset(tables, '\0', sizeof(tables));
		memset(physical, '\0', sizeof(physical));
		physicalAddr = 0;
	}
	
	Pointer tables[4096 / sizeof(Pointer )];
	
	Pointer physical[4096 / sizeof(Pointer )];
	
	Pointer physicalAddr;
	
	void setPhys(Pointer p){physicalAddr = p;}
	
	Pointer index(Pointer vaddr)const
	{
		return ((vaddr>>SHIFT) & (u64int)(0x1FFu));
	}
	
	PageType *getPage(Pointer vaddr)
	{
		auto i = index(vaddr);
		if( ! tables[i] )
		{
			u32int phys = 0;
			
			auto *table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(T), &phys))) T{};
			table->setPhys(phys);			
			tables[i] = reinterpret_cast<u64int>(table);			
			physical[i] = (phys|0x07); // PRESENT, RW, US.			
		}
		return reinterpret_cast<T *>(tables[i])->getPage(vaddr);
	}
};

using PTE = directory<page_table64_t, u64int, 12>;
using PDE = directory<PTE, u64int, 21>;
using PDPTE = directory<PDE, u64int, 30>;
using PML4E = directory<PDPTE, u64int, 39>;

using DIR32 = directory<page_table32_t, u32int, 10>;

static
u64int
pml4eTable()
{
	static PML4E table{};
	return reinterpret_cast<u64int>(&table);
}


static DIR32 * getDIR32()
{
	static DIR32 table{};
	return &table;
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


extern "C"
{

	void *get_directory32()
	{
		return getDIR32();
	}
	
	page32_t *get_page32(u32int address)
	{
		return getDIR32()->getPage(address);
	}
	
	void *init_paging()
	{	
		__asm__ volatile ( "movl %eax, %edx\n movb $0x39, %al\n outb %al, $0xe9 \n movl %edx,%eax");
		void *p = make_page_tables(33554432);
		__asm__ volatile ( "movl %eax, %edx\n movb $0x39, %al\n outb %al, $0xe9 \n movl %edx,%eax");
		return p;
	}

}
