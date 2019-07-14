#ifndef PAGEDIRECTORY_H_INCLUDED
#define PAGEDIRECTORY_H_INCLUDED

template<typename T, const int SHIFT, const int BITS>
struct PageDirectory
{
	using PageType=typename T::PageType;
	using  Pointer=typename PageType::Pointer;
	
	static constexpr Pointer MASK = ((1<<BITS)-1);
	
	PageDirectory()
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
			physical[i] = (phys|0x07); // PRESENT, RW, US.			
		}
		return table->getPage(vaddr);
	}
};

template<class UINT>
class PageTableT
{
public:

	static constexpr decltype(sizeof(int)) NUM_PAGES = 4096 / sizeof(UINT);

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
	
private:
	PageType pages[NUM_PAGES];

};

#endif // PAGEDIRECTORY_H_INCLUDED
