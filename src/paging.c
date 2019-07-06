#include "common.h"
#include "paging.h"
#include "kmalloc.h"
#include "vesavga.h"
#include "frame.h"


// Defined in kheap.c
extern u32int placement_address;

extern void set_page_directory(u32int (*d)[]);
u32int *frames = 0;
u32int nframes = 0;

// The kernel's page directory
page_directory_t *kernel_directory=0;

// The current page directory;
page_directory_t *current_directory=0;

void initialise_paging()
{
	// The size of physical memory. For the moment we
	// assume it is 16MB big.
	u32int mem_end_page = 0x1000000;

	nframes = mem_end_page / 0x1000;
	frames = (u32int*)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));

	// Let's make a page directory.
	kernel_directory = (page_directory_t*)kmalloc_aligned(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;

	// We need to identity map (phys addr = virt addr) from
	// 0x0 to the end of used memory, so we can access this
	// transparently, as if paging wasn't enabled.
	// NOTE that we use a while loop here deliberately.
	// inside the loop body we actually change placement_address
	// by calling kmalloc(). A while loop causes this to be
	// computed on-the-fly rather than once at the start.
	int i = 0;
	while (i < placement_address)
	{
		// Kernel code is readable but not writeable from userspace.
		alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
		i += PAGE_SIZE;
	}
	// Before we enable paging, we must register our page fault handler.
	register_interrupt_handler(14, page_fault);

	// Now, enable paging!
	switch_page_directory(kernel_directory);
}


void switch_page_directory(page_directory_t *dir)
{
    //~ current_directory = dir;
    //~ asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
    //~ u32int cr0;
    //~ asm volatile("mov %%cr0, %0": "=r"(cr0));
    //~ cr0 |= 0x80000000; // Enable paging!
    //~ asm volatile("mov %0, %%cr0":: "r"(cr0));
	
	current_directory = dir;
	set_page_directory( &dir->tablesPhysical );

}

page_t *get_page(u32int address, int make, page_directory_t *dir)
{
	// Turn the address into an index.
	address /= PAGE_SIZE;
	// Find the page table containing this address.
	u32int table_idx = address / 1024;
	if (dir->tables[table_idx]) // If this table is already assigned
	{
		return &dir->tables[table_idx]->pages[address%1024];
	}
	else if(make)
	{
		u32int tmp;
		dir->tables[table_idx] = (page_table_t*)kmalloc_aligned_phys(sizeof(page_table_t), &tmp);
		memset(dir->tables[table_idx], 0, PAGE_SIZE);
		dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
		return &dir->tables[table_idx]->pages[address%1024];
	}
	else
	{
		return NULL;
	}
}

void page_fault(registers_t regs)
{
	// Output an error message.
	//monitor_write("Page fault! ( ");
	char buf[32];
	// A page fault has occurred.
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
