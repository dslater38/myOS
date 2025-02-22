#ifndef KHEAP_H_INCLUDED
#define KHEAP_H_INCLUDED

#include "common.h"
#include "ordered_array.h"
#include "NewObj.h"
#include "Frames.h"
#include <functional>

namespace VM {
	class Manager;
}

// constexpr uint64_t KHEAP_START = 0x00000000C0000000;
constexpr size_t KHEAP_INITIAL_SIZE = 0x200000;
constexpr size_t HEAP_INDEX_SIZE = 0x20000;
constexpr uint32_t HEAP_MAGIC = 0x123890AB;
constexpr size_t HEAP_MIN_SIZE = 0x70000;
constexpr size_t HEAK_MAX_SIZE = 0x200000000;

struct header_t
{
	uint32_t magic{HEAP_MAGIC};
	uint32_t size{0};
	uint8_t is_hole{1};
	header_t(uint32_t size_, uint8_t hole)
		: magic{HEAP_MAGIC}
		, size{size_}
		, is_hole{hole}
		{
		}
};

struct footer_t
{
	uint32_t magic{HEAP_MAGIC};
	header_t	*header{0};
	explicit footer_t(header_t *h)
		: magic{HEAP_MAGIC}
		, header{h}
		{
		}
};

template<>
struct std::less<header_t>
{
	uint8_t operator()(const header_t &a, const header_t& b)
	{
		return a.size < b.size ? 1 : 0;
	}
};

typedef bool (*PageAlloc)(uint64_t startAddress, size_t numPages, bool isKernel, bool isWritable);
typedef bool (*PageFree)(uint64_t startAddress, size_t numPages);

struct heap_t
{
public:	
	heap_t(PageAlloc alloc, PageFree free, uint64_t start, uint64_t end_addr, uint64_t max_size, uint8_t super, uint8_t ro);	
	heap_t(VM::Manager *pMgr, uint64_t start, uint64_t end_addr, uint64_t max_size, uint8_t super, uint8_t ro);	
	static heap_t *create(void *memory, PageAlloc alloc, PageFree free, uint64_t start, uint64_t end_addr, uint64_t max_size, uint8_t supervisor, uint8_t readonly);
	void *alloc(uint64_t size, bool page_align);
	void free(void *p);
	uint64_t blockSize(void *p);

private:
	int64_t find_smallest_hole(uint64_t size, bool page_align);
	void expand(uint64_t new_size);
	uint64_t contract(uint64_t new_size);
	bool allocPages(uint64_t startAddress, size_t numPages, bool isKernel, bool isWritable);
	bool freePages(uint64_t startAddress, size_t numPages);
private:
	ordered_array_t<header_t *> index;
	PageAlloc 		pageAlloc{nullptr};
	PageFree 		pageFree{nullptr};
	uint64_t start_address{0}; // The start of our allocated space.
	uint64_t end_address{0};   // The end of our allocated space. May be expanded up to max_address.
	uint64_t max_address{0};   // The maximum address the heap can be expanded to.
	uint8_t supervisor{0};     // Should extra pages requested by us be mapped as supervisor-only?
	uint8_t readonly{0};       // Should extra pages requested by us be mapped as read-only?
	VM::Manager		*pManager{nullptr};
};

heap_t *initialKernelHeap(VM::Manager &);

#endif // KHEAP_H_INCLUDED
