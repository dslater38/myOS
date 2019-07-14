
#include "common.h"
#include "paging.h"


// The kernel's page directory
//page_directory_t *kernel_directory=0;

// The current page directory;
//page_directory_t *current_directory=0;

// A bitset of frames - used or free.
extern u32int *frames;
extern u32int nframes;

// Defined in kheap.c
extern u32int placement_address;


// Static function to set a bit in the frames bitset
static void set_frame(u32int frame_addr)
{
	u32int frame = frame_addr/PAGE_SIZE;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(u32int frame_addr)
{
	u32int frame = frame_addr/PAGE_SIZE;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static u32int test_frame(u32int frame_addr)
{
	u32int frame = frame_addr/PAGE_SIZE;
	u32int idx = INDEX_FROM_BIT(frame);
	u32int off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static u32int first_frame()
{
	u32int i, j;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
	{
		u32int frame = frames[i];
		if (frame != 0xFFFFFFFF) // nothing free, exit early.
		{			
			// at least one bit is free here.
			for (j = 0; j < 32; j++)
			{
				u32int toTest = (0x1 << j);
				if ( 0 == (frame & toTest) )
				{
					return i*4*8+j;
				}
			}
		}
	}
	return (u32int)(-1);
}

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	if(page->frame == 0)
	{
		u32int idx = first_frame(); // idx is now the index of the first free frame.
		if (idx == (u32int)-1)
		{
		   // PANIC is just a macro that prints a message to the screen then hits an infinite loop.
		   PANIC("No free frames!");
		}
		set_frame(idx*0x1000); // this frame is now ours!
		page->present = 1; // Mark it as present.
		page->rw = (is_writeable)?1:0; // Should the page be writeable?
		page->user = (is_kernel)?0:1; // Should the page be user-mode?
		page->frame = idx;
	}
}

// Function to deallocate a frame.
void free_frame(page_t *page)
{
	u32int frame;
	if(  (frame=page->frame) != 0 )
	{
		clear_frame(frame); // Frame is now free again.
		page->frame = 0x0; // Page now doesn't have a frame.
	}
}

