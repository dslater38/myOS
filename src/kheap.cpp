#include "kheap.h"
#include "common.h"
#include "ordered_array.h"
#include "PageDirectory.h"
#include "NewObj.h"


extern "C"
{
	extern uint64_t p4_table[512];
	extern uint64_t p3_table[512];
	extern uint64_t p2_table[512];
    extern void invalidate_tlb(uint64_t);
}


heap_t::heap_t(PageAlloc alloc, PageFree free, uint64_t start, uint64_t end_addr, uint64_t max_size, uint8_t super, uint8_t ro)
	: index{reinterpret_cast<header_t **>(start), (uint64_t)HEAP_INDEX_SIZE}
	, pageAlloc{alloc}
    , pageFree{free}
	, start_address{start}
	, end_address{end_addr}
	, max_address{max_size}
	, supervisor{super}
	, readonly{ro}
{
    printf("create_heap: start: %016.16lx, end: %016.16lx, max: %016.16lx\n", start, end_addr, max_size);
	// All our assumptions are made on startAddress and endAddress being page-aligned.
	ASSERT(start%0x1000 == 0);
	ASSERT(end_addr%0x1000 == 0);

	// Shift the start address forward to resemble where we can start putting data.
	start_address += sizeof(void *)*HEAP_INDEX_SIZE;
	PAGE_ALIGN(start_address);
		
    printf("hole: %016.16lx\n", start_address);

	// We start off with one large hole in the index.
	header_t *hole = (header_t *)start_address;
	hole->size = end_addr-start_address;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;
	insert_ordered_array(hole, &index);
}

// static 
heap_t *heap_t::create(PageAlloc alloc, PageFree free, uint64_t start, uint64_t end_addr, uint64_t max_size, uint8_t supervisor, uint8_t readonly)
{
    void *memory = kmalloc(sizeof(heap_t));
    heap_t *heap = new(memory) heap_t{alloc, free, start, end_addr, max_size, supervisor, readonly};
    return heap;
	// return New<heap_t>(alloc, free, start, end_addr, max_size, supervisor, readonly);
}

int64_t heap_t::find_smallest_hole(uint64_t size, bool page_align)
{
	// Find the smallest hole that will fit.
	uint64_t iterator = 0;
	auto index_size = index.size();
	for(; iterator<index_size; ++iterator)
	{
		header_t *header = (header_t *)lookup_ordered_array(iterator, &index);
		// If the user has requested the memory be page-aligned
		if (page_align > 0)
		{
			// Page-align the starting point of this header.
			uint64_t location = (uint64_t )header;
			int64_t offset = 0;
			if ( ((location+sizeof(header_t)) & 0xFFFFFFFFFFFFF000ull) != 0)
			{
				offset = 0x1000 /* page size */  - (location+sizeof(header_t))%0x1000;
			}
			int64_t hole_size = (int64_t)header->size - offset;
			// Can we fit now?
			if (hole_size >= (int64_t)size)
			{
				break;
			}
		}
		else if (header->size >= size)
		{
			break;
		}
	}
	// Why did the loop exit?
	if (iterator == index_size)
	{
		return -1; // We got to the end and didn't find anything.
	}
	else
	{
		return iterator;
	}
}

void heap_t::expand(uint64_t new_size)
{
	// Sanity check.
	ASSERT(new_size > (end_address - start_address));
	// Get the nearest following page boundary.
	if ( (new_size&0x0000000000000FFFull) != 0)
	{
		new_size &= 0xFFFFFFFFFFFFF000ull;
		new_size += 0x1000ull;
	}
	// Make sure we are not overreaching ourselves.
	ASSERT(start_address+new_size <= max_address);

	// This should always be on a page boundary.
	auto old_size = end_address-start_address;
	auto i = old_size;
	if ( pageAlloc(start_address + i, new_size - old_size, supervisor, !readonly) )
	{
		end_address = start_address+new_size;
	}
    else
    {
        PANIC("FAILED TO EXPAND THE KERNEL HEAP\n");
    }
}

uint64_t heap_t::contract(uint64_t new_size)
{
    // Sanity check.
    ASSERT(new_size < end_address-start_address);

    // Get the nearest following page boundary.
    if (new_size&0x1000)
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    // Don't contract too far!
    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;

    auto old_size = end_address-start_address;
    auto i = old_size - 0x1000;
    if( pageFree(start_address, i))
    {
        end_address = start_address + new_size;
        return new_size;
    }
    else
    {
        PANIC("KERNEL PAGE FREE FAILED.\n");
    }
    
    return 0;
}


void *heap_t::alloc(uint64_t size, bool page_align)
{

    // Make sure we take the size of header/footer into account.
    auto new_size = size + sizeof(header_t) + sizeof(footer_t);
    // Find the smallest hole that will fit.
    auto iterator = find_smallest_hole(new_size, page_align);

    if (iterator == -1) // If we didn't find a suitable hole
    {
        // Save some previous data.
        auto old_length = end_address - start_address;
        auto old_end_address = end_address;

        // We need to allocate some more space.
        expand(old_length+new_size);
        auto new_length = end_address-start_address;

        // Find the endmost header. (Not endmost in size, but in location).
        iterator = 0;
        // Vars to hold the index of, and value of, the endmost header found so far.
        auto idx = -1; auto value = 0x0;
        auto index_size = index.size();
        while (iterator < index_size)
        {
            auto tmp = (uint64_t)lookup_ordered_array(iterator, &index);
            if (tmp > value)
            {
                value = tmp;
                idx = iterator;
            }
            iterator++;
        }

        // If we didn't find ANY headers, we need to add one.
        if (idx == -1)
        {
            header_t *header = (header_t *)old_end_address;
            header->magic = HEAP_MAGIC;
            header->size = new_length - old_length;
            header->is_hole = 1;
            footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
            insert_ordered_array(header, &index);
        }
        else
        {
            // The last header needs adjusting.
            header_t *header = lookup_ordered_array(idx, &index);
            header->size += new_length - old_length;
            // Rewrite the footer.
            footer_t *footer = (footer_t *) ( (uint64_t)header + header->size - sizeof(footer_t) );
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }
        // We now have enough space. Recurse, and call the function again.
        return alloc(size, page_align);
    }

    header_t *orig_hole_header = (header_t *)lookup_ordered_array(iterator, &index);
    auto orig_hole_pos = (uint64_t)orig_hole_header;
    auto orig_hole_size = orig_hole_header->size;
    // Here we work out if we should split the hole we found into two parts.
    // Is the original hole size - requested hole size less than the overhead for adding a new hole?
    if (orig_hole_size-new_size < sizeof(header_t)+sizeof(footer_t))
    {
        // Then just increase the requested size to the size of the hole we found.
        size += orig_hole_size-new_size;
        new_size = orig_hole_size;
    }

    // If we need to page-align the data, do it now and make a new hole in front of our block.
    if (page_align && orig_hole_pos&0xFFFFF000)
    {
        auto new_location   = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
        header_t *hole_header = (header_t *)orig_hole_pos;
        hole_header->size     = 0x1000 /* page size */ - (orig_hole_pos&0xFFF) - sizeof(header_t);
        hole_header->magic    = HEAP_MAGIC;
        hole_header->is_hole  = 1;
        footer_t *hole_footer = (footer_t *) ( (uint64_t)new_location - sizeof(footer_t) );
        hole_footer->magic    = HEAP_MAGIC;
        hole_footer->header   = hole_header;
        orig_hole_pos         = new_location;
        orig_hole_size        = orig_hole_size - hole_header->size;
    }
    else
    {
        // Else we don't need this hole any more, delete it from the index.
        remove_ordered_array(iterator, &index);
    }

    // Overwrite the original header...
    header_t *block_header  = (header_t *)orig_hole_pos;
    block_header->magic     = HEAP_MAGIC;
    block_header->is_hole   = 0;
    block_header->size      = new_size;
    // ...And the footer
    footer_t *block_footer  = (footer_t *) (orig_hole_pos + sizeof(header_t) + size);
    block_footer->magic     = HEAP_MAGIC;
    block_footer->header    = block_header;

    // We may need to write a new hole after the allocated block.
    // We do this only if the new hole would have positive size...
    if (orig_hole_size - new_size > 0)
    {
        header_t *hole_header = (header_t *) (orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
        hole_header->magic    = HEAP_MAGIC;
        hole_header->is_hole  = 1;
        hole_header->size     = orig_hole_size - new_size;
        footer_t *hole_footer = (footer_t *) ( (uint64_t)hole_header + orig_hole_size - new_size - sizeof(footer_t) );
        if ((uint64_t)hole_footer < end_address)
        {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        // Put the new hole in the index;
        insert_ordered_array(hole_header, &index);
    }
    
    // ...And we're done!
    return (void *) ( (uint64_t)block_header+sizeof(header_t) );
}

void heap_t::free(void *p)
{
    // Exit gracefully for null pointers.
    if (p == 0)
        return;

    // Get the header and footer associated with this pointer.
    header_t *header = (header_t*) ( (uint64_t)p - sizeof(header_t) );
    footer_t *footer = (footer_t*) ( (uint64_t)header + header->size - sizeof(footer_t) );

    // Sanity checks.
    ASSERT(header->magic == HEAP_MAGIC);
    ASSERT(footer->magic == HEAP_MAGIC);

    // Make us a hole.
    header->is_hole = 1;

    // Do we want to add this header into the 'free holes' index?
    char do_add = 1;

    // Unify left
    // If the thing immediately to the left of us is a footer...
    footer_t *test_footer = (footer_t*) ( (uint64_t)header - sizeof(footer_t) );
    if (test_footer->magic == HEAP_MAGIC &&
        test_footer->header->is_hole == 1)
    {
        auto cache_size = header->size; // Cache our current size.
        header = test_footer->header;     // Rewrite our header with the new one.
        footer->header = header;          // Rewrite our footer to point to the new header.
        header->size += cache_size;       // Change the size.
        do_add = 0;                       // Since this header is already in the index, we don't want to add it again.
    }

    // Unify right
    // If the thing immediately to the right of us is a header...
    header_t *test_header = (header_t*) ( (uint64_t)footer + sizeof(footer_t) );
    if (test_header->magic == HEAP_MAGIC &&
        test_header->is_hole)
    {
        header->size += test_header->size; // Increase our size.
        test_footer = (footer_t*) ( (uint64_t)test_header + // Rewrite it's footer to point to our header.
                                    test_header->size - sizeof(footer_t) );
        footer = test_footer;
        // Find and remove this header from the index.
        auto iterator = 0;
        auto index_size = index.size();
        while ( (iterator < index_size) &&
                (lookup_ordered_array(iterator, &index) != (void*)test_header) )
            iterator++;

        // Make sure we actually found the item.
        ASSERT(iterator < index_size);
        // Remove it.
        remove_ordered_array(iterator, &index);
    }

    // If the footer location is the end address, we can contract.
    if ( (uint64_t)footer+sizeof(footer_t) == end_address)
    {
        auto old_length = end_address-start_address;
        auto new_length = contract( (uint64_t)header - start_address);
        // Check how big we will be after resizing.
        if (header->size - (old_length-new_length) > 0)
        {
            // We will still exist, so resize us.
            header->size -= old_length-new_length;
            footer = (footer_t*) ( (uint64_t)header + header->size - sizeof(footer_t) );
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }
        else
        {
            // We will no longer exist :(. Remove us from the index.
            auto iterator = 0;
            auto index_size = index.size();
            while ( (iterator < index_size) &&
                    (lookup_ordered_array(iterator, &index) != (void*)test_header) )
                iterator++;
            // If we didn't find ourselves, we have nothing to remove.
            if (iterator < index_size)
                remove_ordered_array(iterator, &index);
        }
    }

    // If required, add us to the index.
    if (do_add == 1)
        insert_ordered_array(header, &index);

}

static PML4E_4K *pmle4 = reinterpret_cast<PML4E_4K *>(p4_table);
uint8_t frames_buffer[sizeof(Frames<uint64_t>)];
Frames<uint64_t> *frames = nullptr; // {2ull*1024ull*1024ull*1024ull};

heap_t *kernelHeap = nullptr;

void initHeap()
{
    frames = New<Frames<uint64_t>>( (uint64_t)(2ull*1024ull*1024ull*1024ull) );
    // new (reinterpret_cast<void *>(frames_buffer)) Frames<uint64_t> {2ull*1024ull*1024ull*1024ull};
    frames->mapInitialPages(512);
    kernelHeap = initialKernelHeap();
}


Page4K<uint64_t> *getPage(void *vaddr)
{
    return pmle4->findPage(reinterpret_cast<uint64_t>(vaddr));
}

bool allocPages(uint64_t startAddress, size_t numPages, bool isKernel, bool isWritable)
{
    debug_out("numPages: %d\n", numPages);
    for( auto i=0; i<numPages; ++i, startAddress+=0x1000)
    {
        debug_out("mapped: 0x%016.16lx\n",startAddress);
        auto * page = pmle4->getPage(startAddress);
        frames->alloc(page, isKernel, isWritable);
        invalidate_tlb((uint64_t)&startAddress);
    }
    return true;
}

bool freePages(uint64_t startAddress, size_t numPages)
{
    for( auto i=0; i<numPages; ++i,startAddress+=0x1000)
    {
        auto * page = pmle4->findPage(startAddress);
        ASSERT(page != nullptr);
        frames->free(page);
    }
    return true;
}

heap_t *initialKernelHeap()
{
    debug_out("initialKernelHeap\n");
    allocPages(KHEAP_START, KHEAP_INITIAL_SIZE>>12, 0, 1);
    pmle4->dump();
	return heap_t::create(allocPages, freePages, KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, HEAK_MAX_SIZE, 0, 0 );
}