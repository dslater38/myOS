#ifndef PAGEDIRECTORY_H_INCLUDED
#define PAGEDIRECTORY_H_INCLUDED

template<typename T, const int SHIFT, const int BITS>
struct PageDirectory
{
	using PageType=typename T::PageType;
	using  Pointer=typename PageType::Pointer;
	
	static constexpr Pointer MASK = ((1<<BITS)-1);
	static constexpr decltype(sizeof(int)) NUM_ENTRIES = 4096 / sizeof(Pointer);
	
	Pointer physical[NUM_ENTRIES];
	Pointer tables[NUM_ENTRIES];
	Pointer physicalAddr;
	
	
	PageDirectory()
	{
		memset(tables, '\0', sizeof(tables));
		memset(physical, '\0', sizeof(physical));
		physicalAddr = 0;
		// printf("PageDirectory<,%d,%d> ctor, this == 0x%08.8x\n", SHIFT,BITS, (u32int)this);
	}
	
	void setPhys(Pointer p)
	{
		// printf("setPhys: this: 0x%08.8x, phys: 0x%08.8x\n", (u32int)this, (u32int)p);
		physicalAddr = p;
	}
	
	Pointer index(Pointer vaddr)const
	{
		return ((vaddr>>(SHIFT+12)) & MASK);
	}
	
	T *operator[](int n)const
	{
		return reinterpret_cast<T *>(tables[n]);
	}
	
	PageType *getPage(Pointer vaddr)
	{
		auto i = index(vaddr);
		auto *table = (*this)[i];
		if( table == nullptr)
		{
			u32int phys = 0;
			
			table = new(reinterpret_cast<void *>(kmalloc_aligned_phys(sizeof(T), &phys))) T{};
			table->setPhys(phys);			
			tables[i] = reinterpret_cast<Pointer>(table);			
			physical[i] = static_cast<Pointer>(phys|0x03); // PRESENT, RW, US.			
		}
		return table->getPage(vaddr);
	}
	
	void dump()const
	{
		printf32("PageDirectory: this 0x%08.8x, phys 0x%08.8x\n", (u32int)this ,(u32int)physicalAddr);
		for (auto i = 0u; i < NUM_ENTRIES; ++i)
		{
			printf32("entry: %d == 0x%08.8x (0x%08.8x)\n", i, (u32int)tables[i], (u32int)physical[i]);
			if (tables[i])
			{
				reinterpret_cast<T *>(tables[i])->dump();
			}
		}
		printf32("===========================================\n");
	}
};

template<class UINT>
class PageTableT
{
public:

	static constexpr decltype(sizeof(int)) NUM_PAGES = 4096 / sizeof(UINT);

	PageTableT()
	{
		memset(pages, '\0', sizeof(pages));
	}

	using PageType=PageT<UINT>;

	PageType *getPage(UINT vaddr)
	{
		return &(pages[ (vaddr>>12) & (NUM_PAGES-1)]);
	}
	
	void setPhys(u32int){}

	UINT operator[](int n)
	{
		return *(UINT *)(&pages[n]);
	}
	
	void dump()
	{
		for (auto i = 0u; i < NUM_PAGES; ++i)
		{
			printf32("\tpage: %d == 0x%08.8x\n", i, *(u32int *)&(pages[i]) );
		}
		printf32("\t+++++++++++++++++++++++\n");
	}
	
	
private:
	PageType pages[NUM_PAGES];

};

#endif // PAGEDIRECTORY_H_INCLUDED
