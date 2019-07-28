#ifndef PAGEDIRECTORY_H_INCLUDED
#define PAGEDIRECTORY_H_INCLUDED

#include "kmalloc.h"
#include "PageT.h"
#include "memcpy.h"

template<typename T, const int SHIFT, const int BITS>
struct PageDirectory
{
	using PageType=typename T::PageType;
	using  Pointer=typename PageType::Pointer;

	static constexpr int bits=BITS;
	static constexpr Pointer MASK = ((1<<BITS)-1);
	static constexpr decltype(sizeof(int)) NUM_ENTRIES = 4096 / sizeof(Pointer);
	
	static constexpr  size_t OFFSET_BITS=T::OFFSET_BITS;
	
	static constexpr bool is_huge=false;
	
	static constexpr size_t shift=T::shift+BITS;
	
	static constexpr Pointer ATTRS = ((OFFSET_BITS >12 ) ? 0x43 : 0x03);

	Pointer physical[NUM_ENTRIES];
//	Pointer tables[NUM_ENTRIES];
//	Pointer physicalAddr;


	PageDirectory()
	{
//		memset32(tables, '\0', sizeof(tables));
		// memset(physical, '\0', sizeof(physical));
		for( auto i=0; i<NUM_ENTRIES; ++i )
		{
			physical[i] = static_cast<Pointer>(0);
		}
//		physicalAddr = 0;
		// printf("PageDirectory<,%d,%d> ctor, this == 0x%08.8x\n", SHIFT,BITS, (uint32_t)this);
	}

	void setPhys(Pointer p)
	{
		// printf("setPhys: this: 0x%08.8x, phys: 0x%08.8x\n", (uint32_t)this, (uint32_t)p);
//		physicalAddr = p;
	}

	Pointer index(Pointer vaddr)const
	{
		return ((vaddr>>(SHIFT)) & MASK);
	}

	T *operator[](int n)const
	{
		const Pointer MASK = (~((Pointer)(0xFFF)));
		return reinterpret_cast<T *>(  MASK & physical[n] );
	}

	PageType *getPage(Pointer vaddr)
	{
		auto *page = findPage(vaddr);
		if( page == nullptr)
		{
			uint32_t phys = 0;
			auto i = index(vaddr);
			auto *table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(T), &phys))) T{};
//			table->setPhys(phys);
//			tables[i] = reinterpret_cast<Pointer>(table);
			physical[i] = static_cast<Pointer>(phys|ATTRS); // PRESENT, RW, US.
			page = table->getPage(vaddr);
		}
		return page;
	}

	PageType *findPage(Pointer vaddr)
	{
		auto i = index(vaddr);
		auto n = physical[i] ;

		if( (n & 0xFFFFFFFCull) != 0 )
		{
			PageDirectoryEntryT<Pointer> &entry  = PageDirectoryEntryT<Pointer>::toEntry(n);
		
			ASSERT( entry.present == 1 );
		
			auto *table = (T *)(n & 0xFFFFFFFCull);
		
			return table ? table->getPage(vaddr) : nullptr;
		}
		return nullptr;
	}

	void dump()const
	{
#if 0		
		printf("PageDirectory: this 0x%08.8x, phys 0x%08.8x\n", (uint32_t)this ,(uint32_t)physicalAddr);
		for (auto i = 0u; i < NUM_ENTRIES; ++i)
		{
			printf("entry: %d == 0x%08.8x (0x%08.8x)\n", i, (uint32_t)tables[i], (uint32_t)physical[i]);
			if (tables[i])
			{
				reinterpret_cast<T *>(tables[i])->dump();
			}
		}
		printf("===========================================\n");
#endif // 0		
	}
};

template<class UINT, const int BITS>
class PageTableT
{
public:

	static constexpr decltype(sizeof(int)) NUM_PAGES = 4096 / sizeof(UINT);

	PageTableT()
	{
		UINT *p = reinterpret_cast<UINT *>(pages);
		for( auto i=0; i<NUM_PAGES; ++i )
		{
			p[i] = 0u;
		}
	}

	using PageType=PageT<UINT>;
	static constexpr  size_t OFFSET_BITS=PageType::OFFBITS;

	PageType *getPage(UINT vaddr)
	{
		return &(pages[( (vaddr >> 12) & 0x1FF)]);
	}

	void setPhys(uint32_t){}

	UINT operator[](int n)
	{
		return *(UINT *)(&pages[n]);
	}

	void dump()
	{
		for (auto i = 0u; i < NUM_PAGES; ++i)
		{
			printf("\tpage: %d == 0x%08.8x\n", i, *(uint32_t *)&(pages[i]) );
		}
		printf("\t+++++++++++++++++++++++\n");
	}


private:
	PageType pages[NUM_PAGES];

};

template<class T, const int BITS=9>
using PageTable64 = PageTableT<uint64_t,BITS>;

template<typename T, const int SHIFT>
using PageDirectory64 = PageDirectory<T, SHIFT, 9>;

using PTE_=PageTable64<uint64_t>;

template<typename T>
using PDE_ = PageDirectory64<T, 21>;

template<const int BITS=10>
using PageTable32 = PageTableT<uint32_t,BITS>;

template<typename T, const int SHIFT>
using PageDirectory32 = PageDirectory<T, SHIFT, 10>;

template<typename T>
using PDPTE_ = PageDirectory64<T, 30>;

template<typename T>
using PML4E_=PageDirectory64<T,39>;




using PageTable32_4M = PageDirectory32<Page4M, 22>;




using PTE32 = PageTable32<10>;

template<typename T>
using PDE32 = PageDirectory32<T, 22>;

using PDE32_4K = PDE32<PTE32>;
using PDE32_2M = PageTable32<22>;

// 32-bit PAE paging ( 4K or 2MB pages )
using PDPTE_PAE = PageDirectory32<PDE32<PTE32>, 30>;
using PDPTE_PAE_ = PageDirectory<PageDirectory<Page4K<uint32_t>, 12,10>,30,10>;


// 32-bit 4K page:
using Page32_4K=Page4K<uint32_t>;
// 32-bit 2M page
using Page32_2M=Page2M<uint32_t>;
// 32-bit 4M page: - defined in PageT.h
// Page4M=PageT<uint32_t, 22>;

// 64 bit 4K page
using Page64_4K=Page4K<uint64_t>;
// 64-bit 2M page
using Page64_2M=Page2M<uint64_t>;
// 64-bit 1G page
using Page64_1G=Page1G<uint64_t>;

// 32-bit 4k page table
using PTE_32_4K=PageDirectory<Page32_4K,Page32_4K::shift,10>;
using PDE_32_4K=PageDirectory<PTE_32_4K,PTE_32_4K::shift,10>;

//32-bit 4M page table
using PDE_32_4M=PageDirectory<Page4M,Page4M::shift,10>;

// 32-bit 4k PAE table
using PTE_PAE_4K=PageDirectory<Page32_4K,Page32_4K::shift, 9>;
using PDE_PAE_4K=PageDirectory<PTE_PAE_4K,PTE_PAE_4K::shift,9>;
using PDPTE_PAE_4K=PageDirectory<PDE_PAE_4K,PDE_PAE_4K::shift,2>;

// 32-bit 2M PAE Table
using PDE_PAE_2M=PageDirectory<Page32_2M,Page32_2M::shift, 9>;
using PDPTE_PAE_2M=PageDirectory<PDE_PAE_2M,PDE_PAE_2M::shift,2>;

// 64-bit 4k page table
using PTE_64_4K=PageDirectory<Page64_4K,Page64_4K::shift,9>;
using PDE_64_4K=PageDirectory<PTE_64_4K,PTE_64_4K::shift,9>;
using PDPTE_64_4K=PageDirectory<PDE_64_4K,PDE_64_4K::shift,9>;
using PML4E_4K=PageDirectory<PDPTE_64_4K,PDPTE_64_4K::shift,9>;

// 64-bit 2M page table
using PDE_64_2M=PageDirectory<Page64_2M,Page64_2M::shift,9>;
using PDPTE_64_2M=PageDirectory<PDE_64_2M,PDE_64_2M::shift,9>;
using PML4E_2M=PageDirectory<PDPTE_64_2M,PDPTE_64_2M::shift,9>;

// 64-bit 1G page table
using PDPTE_64_1G=PageDirectory<Page64_1G,Page64_1G::shift,9>;
using PML4E_1G=PageDirectory<PDPTE_64_1G,PDPTE_64_1G::shift,9>;


#endif // PAGEDIRECTORY_H_INCLUDED
