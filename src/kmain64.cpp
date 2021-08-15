#include "common.h"
#include "vesavga.h"
#include "serial.h"
#include "AtaController.h"
#include "multiboot2.h"
#include "PageDirectory.h"
#include "Frames.h"
#include "VmmPageStack.h"
#include "TextFrameBuffer.h"
#include "MultiBootInfoHeader.h"
#include "BootInformation.h"
#include "vfs.h"
#include "timer.h"
#include "cpu.h"
#include "cpuinfo.h"
#include "VMManager.h"
#include "fat.h"
#include "ata.h"
#include "mbr.h"
#include <array>

void init_idt64_table();

static std::array<uint8_t, 512> partData{};

// uint32_t mboot_header=0;

volatile int foo___ = 0;
static void test_page_fault();
static void reportPartition(int which);

uint32_t indent=0;

void cmain (BootInformation &bootInfo, const MultiBootInfoHeader *addr);

extern Frames<uint64_t> *initHeap(VM::Manager &mgr);
void mapMemory(Frames<uint64_t> *frames, const multiboot_tag_mmap *mmap);
// uint64_t RTC_currentTime();
void init_rct_interrupts();
// void init_timer(uint32_t frequency);
extern void init_keyboard_handler();
void testVmmPageStack(const MultiBootInfoHeader *mboot_header);

static void detectControllers();
// static void detectControllersOld();
static void dumpPageTables();
static void accessP4Table();
extern const multiboot_tag *findMultiBootInfoHeaderTag(const MultiBootInfoHeader *addr, multiboot_uint32_t type);

static const char *yn(uint32_t n)
{
	return n ? "YES" : "no";
}

extern "C"
{
	extern PML4E_4K *p4_table;
	void initTextFrameBuffer();
	extern uint64_t placement_address;
	void __libc_init_array (void);
	extern void startup_data_start();
	extern void startup_data_end();
	extern void report_idt_info();

	extern uint64_t p3_gb_mapped_table[512];

	extern void invalidate_all_tlbs();

	void kmain64(uint32_t magic, const MultiBootInfoHeader *mboot_header)
	{
		// Entry - at this point, we're in 64-bit long mode with a basic
		// page table that identity maps the first 2 MB or RAM
		// install our interrupt handlers
//		init_idt64_table();

//		__libc_init_array();

		/*  Am I booted by a Multiboot-compliant boot loader? */
		if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		{
			char buffer[40];
			sprintf(buffer,"Invalid magic number: 0x%x\n", (unsigned) magic);
			PANIC (buffer);
		}


		if(mboot_header && placement_address < reinterpret_cast<uint64_t>(mboot_header))
		{
			// if the mboot header is higher up in memory that placement_address
			// then copy the mboot header down to the bottom of our memory
			// so we can allow placement_address to run across the header's memory.
			auto size = mboot_header->size;
			memcpy(reinterpret_cast<void *>(placement_address), reinterpret_cast<const void *>(mboot_header), size);
			mboot_header = reinterpret_cast<const MultiBootInfoHeader *>(placement_address);
			placement_address += size;
		}
		initTextFrameBuffer();
		set_foreground_color((uint8_t)TextColors::GREEN);
		set_background_color((uint8_t)TextColors::BLACK);

		printf("Hello World from 64-bit long mode!!!!!\n");

		auto success = init_serial(2, BAUD_115200, BITS_8, PARITY_NONE, NO_STOP_BITS) ;
		if (success == SUCCESS)
		{
			printf("Initialized COM2\n");
		}

		debug_out("Startup Data Block: start 0x%016.16lx, end: 0x%016.16lx\n",(uint64_t)&startup_data_start, (uint64_t)&startup_data_end);
		report_idt_info();
		

		// process the mboot header.
		BootInformation bootInfo{};
		if( mboot_header != 0)
		{
			printf("process mboot_header\n");
			cmain(bootInfo, mboot_header);
//			mapMemory(frames, bootInfo.mmap);
		}
		else
		{
			PANIC("mboot_header is NULL\n");
		}
		
		
    	// const auto *tag = findMultiBootInfoHeaderTag(mboot_header, MULTIBOOT_TAG_TYPE_MMAP);
		// VM::Manager mgr {reinterpret_cast <const multiboot_tag_mmap *>(tag)};
		VM::Manager mgr{bootInfo.mmap};
		
		
		auto *frames = initHeap(mgr);
		printf("Heap Initialized...\n");

		success = init_serial(1, BAUD_115200, BITS_8, PARITY_NONE, NO_STOP_BITS) ;
		if( success == SUCCESS )
		{
			printf("Initialized COM1 port\n");
		}

		printf("COM1: %s,\tCOM2: %s\n",identify_uart(1), identify_uart(2));
		printf("COM3: %s,\tCOM4: %s\n",identify_uart(3), identify_uart(4));
		
	
//		dumpPageTables();
		// detect ata disks & controllers.
		detectControllers();
		
		// reportPartition(0);
		//reportPartition(1);

		// printf("RTC Current Time:\n");
		// RTC_currentTime();
		
		Cpu cpu{};
		printf("Cpuid: MaxRequest: %u, Vendor ID: %s\n", cpu .MaxRequestLevel(), cpu.VendorId());
	
		
//		testVmmPageStack(mboot_header);

		init_timer(1);
		init_rct_interrupts();
		init_keyboard_handler();
		asm("sti");

		CpuInfo info{};
		getCpuInfo(info);

		printf("Vendor Id String: %s\n", info.vendorId);
		printf("Stepping: %d, Model: %d, Family: %d, Type: %d, ExtendedModel: %d, ExtendedFamily: %d\n",
		info.stepping, info.model, info.family, info.processor_type, info.extendedModelId, info.extendedFamilyId);
		printf("Global Page Support: %s, SYSCALL support: %s, 1GB Page Support %s\n", yn(info.pge), yn(info.syscall), yn(info.page1gb));
		printf("4MB Page Support: %s, PAE support: %s, IA-32e Support: %s\n", yn(info.pse), yn(info.pae), yn(info.intel64));

		const char *str = getBrandString(info.brand_index);
		if( str )
		{
			printf("Brand String: %s\n", str);
		}

		auto &ctl = AtaController::secondary();
		
		GenericMBR mbr{};
		ctl.read(AtaController::Drive::Primary, &mbr, 0, sizeof(GenericMBR));
		
		printf("MBR: active: %s, type: %u\n", mbr.partitions[0].active() ? "true" : "false", mbr.partitions[0].type());
		auto start = mbr.partitions[0].first();
		printf("MBR: Start CHS: (%u/%u/%u)\n", start.cylinder, start.head, start.sector);
		auto last = mbr.partitions[0].last();
		printf("MBR: End CHS: (%u/%u/%u)\n", last.cylinder, last.head, last.sector);
		printf("MBR: firstLBA: %u\n", mbr.partitions[0].firstLBA());
		printf("MBR: sectorCount: %u\n", mbr.partitions[0].sectorCount());
		printf("Logical Heads: %d, Logical Sectors: %d\n", mbr.partitions[0].numHeads(), mbr.partitions[0].numSectors());
		
		FATFileSystem fs {};
		memset(&fs, 0, sizeof(fs));
		fs.ctl = &ctl;
		fs.d = AtaController::Drive::Primary;
		fs.offset = mbr.partitions[0].firstLBA();
		auto &boot = fs.boot;
		
		ctl.read(AtaController::Drive::Primary, &boot, mbr.partitions[0].firstLBA(), sizeof(BootBlock));
		printf("BootBlock: BytesPerBlock %d, ReservedBlocks %d, NumRootDirEntries %d, TotalNumBlocks: %d\n",
		boot.BytesPerBlock(), boot.ReservedBlocks(), boot.NumRootDirEntries(), boot.TotalNumBlocks());
		printf("NumBlocksFat1: %d, NumBlocksPerTrack %d, NumHeads: %d, NumHiddenBlocks: %d\n",
		boot.NumBlocksFat1(), boot.NumBlocksPerTrack(), boot.NumHeads(), boot.NumHiddenBlocks());
		printf("TotalBlocks: %d, PhysDriveNo: %d, VolumeSerialNumber: %04.4x-%04.4x, FsId: %8.8s, BlockSig: %x\n",
		boot.TotalBlocks(), boot.PhysDriveNo(), (boot.VolumeSerialNumber() & 0xFFFF0000)>>16, boot.VolumeSerialNumber() & 0x0000FFFF, boot.FsId(), boot.BlockSig());
		printf("Volume Label: %11.11s\n", boot.volume_label);
		
		dump_fat_table(fs);
		dump_root_dir(fs);
		print_file(fs, "AUTOEXEC  ","BAT");

		if( info.apic )
		{
			printf("APIC detected...\n");
		}
#if 0
//		asm("cli");
		BootBlock boot{};
		read(&boot, 0, sizeof(BootBlock));
		printf("BootBlock: BytesPerBlock %d, ReservedBlocks %d, NumRootDirEntries %d, TotalNumBlocks: %d\n",
		boot.BytesPerBlock(), boot.ReservedBlocks(), boot.NumRootDirEntries(), boot.TotalNumBlocks());
		printf("NumBlocksFat1: %d, NumBlocksPerTrack %d, NumHeads: %d, NumHiddenBlocks: %d\n",
		boot.NumBlocksFat1(), boot.NumBlocksPerTrack(), boot.NumHeads(), boot.NumHiddenBlocks());
		printf("TotalBlocks: %d, PhysDriveNo: %d, VolumeSerialNumber: %04.4x-%04.4x, FsId: %8.8s, BlockSig: %x\n",
		boot.TotalBlocks(), boot.PhysDriveNo(), (boot.VolumeSerialNumber() & 0xFFFF0000)>>16, boot.VolumeSerialNumber() & 0x0000FFFF, boot.FsId(), boot.BlockSig());
		printf("Volume Label: %11.11s\n", boot.volume_label);

		// dump_fat_table(boot);
		dump_root_dir(boot);
		printf("Dumping DRVSPACE.BIN\n");
		print_clusters(boot, 578);
		print_file(boot, "README  ","TXT");
#endif // 0

		while(true)
		{
			// asm("sti");
			asm("hlt");
		}

		// test_page_fault();


	}

}

extern Page4K *getPage(void *vaddr);

void mapMemory(Frames<uint64_t> *frames, const multiboot_tag_mmap *mmap)
{
	const uint8_t *end = reinterpret_cast<const uint8_t *>(mmap) + mmap->size;
	for (auto *entry = mmap->entries;
		 reinterpret_cast<const uint8_t *>(entry) < end;
		 entry = reinterpret_cast<const multiboot_memory_map_t *> (reinterpret_cast<const uint8_t *>(entry) + mmap->entry_size))
	{
		// mark anything that's not available as in use
		// so we don't try to use these addresses.
		if( entry->type != MULTIBOOT_MEMORY_AVAILABLE )
		{
			uint64_t nPages = (entry->len>>12);
			frames->markFrames(entry->addr, nPages);
			auto addr = entry->addr;
			for( auto i=0ull; i<nPages; ++i, addr += 0x1000 )
			{
				auto *page = getPage(reinterpret_cast<void *>(addr));
				if( page )
				{
					page->rw = 0; 	// mark the page as read-only
					page->user = 0;	// only kernel access
				}
			}
		}
	}
}

static void test_page_fault()
{
	printf("Testing Page Fault\n");
	uint32_t *ptr = (uint32_t*)0xA0000000;
	uint32_t do_page_fault = *ptr;
	monitor_write_dec(do_page_fault);
	monitor_write("After page fault! SHOULDN'T GET HERE \n");
}

static void SetGlobalPageBits()
{
	PTE_64_4K *pte = reinterpret_cast<PTE_64_4K *>(0xfffffffffffff000);
	printf("pte->physical[511] == 0x%016.16lx\n", pte->physical[511]);
	pte->physical[511] |= (1<<8);
	// make the recursive entry in plme4 global
}

static void checkMem(uint8_t *alloc, bool init)
{
	for( auto i=0; i<512; ++i )
	{
		if(alloc[i] != 0xCB)
		{
			printf("Overwrote memry at offset %u\n", i);
		}
		if(!init)
		{
			if(alloc[i+512] == 0xAE)
			{
				printf("disk read didn't overwrite memory at %u\n", i);
			}
		}
		if(alloc[i+1024] != 0xFD)
		{
			printf("Overwrote memry at offset %u\n", i+1024);
		}
	}
}

static void reportPartition(int which)
{
	// uint8_t *buffer = partData.data();
	printf("Allocating 3 pages for reportPArtition\n");
	
	uint8_t *alloc = static_cast<uint8_t *>(kmalloc(512*3));
	if(!alloc)
	{
		PANIC("Memory allocation error.");
	}
	memset(alloc,0xCB,512);
	memset(alloc+512,0xAE,512);
	memset(alloc+1024,0xFD,512);
	checkMem(alloc, true);
	printf("Check before reading disk\n");
	auto *buffer = alloc + 512;
	auto &ctl = which == 0 ? AtaController::primary() : AtaController::secondary();

	auto read = ctl.read(AtaController::Drive::Primary, buffer, 0, 512);
	if(read!=512)
	{
		printf("ctl.read() failed. only got %lu bytes", read);
	}
	else
	{
		auto *part1 = buffer + 446;
		uint8_t status = part1[0];
		printf("Status: %d\n", status);
		uint8_t head = part1[1];
		uint8_t b2 = part1[2];
		uint8_t b3 = part1[3];

		uint16_t cyl = (static_cast<uint16_t>(0xC0 & b2)<<8)|static_cast<uint16_t>(b3);
		uint8_t sector = (b2 & 0x3F);
		printf("Start-C/H/S %d/%d/%d\n", cyl, head, sector);
		printf("Type: %x\n", part1[4]);
		head = part1[5];
		b2 = part1[6];
		b3 = part1[7];
		cyl = (static_cast<uint16_t>(0xC0 & b2)<<8)|static_cast<uint16_t>(b3);
		sector = (b2 & 0x3F);
		printf("End-C/H/S %d/%d/%d\n", cyl, head, sector);
		uint32_t lba = *reinterpret_cast<uint32_t *>(part1 + 8);
		printf("LBA of first sector: %u (%x)\n", lba, lba);
		uint32_t sectorCount = *reinterpret_cast<uint32_t *>(part1 + 0x0C);
		printf("sector count: %u (%x)\n", sectorCount, sectorCount);
	}
	checkMem(alloc, false);
	printf("Check AFTER reading disk\n");
	printf("Freeing memory\n");
	kfree(alloc);
}

static void reportController(const char *format, AtaController::AtaDeviceType type)
{
	bool detected = (type!=AtaController::AtaDeviceType::UNKNOWN);
	printf(format, detected ? "present" : "absent");
	if(detected)
	{
		const char *sType=nullptr;
		switch(type)
		{
        	case AtaController::AtaDeviceType::PATAPI:
				sType="Parallel ATAPI (PATAPI)";
				break;
        	case AtaController::AtaDeviceType::SATAPI:
				sType="Serial ATAPI (SATAPI)";
				break;
        	case AtaController::AtaDeviceType::PATA:
				sType="Parallel ATA (PATA)";
				break;
        	case AtaController::AtaDeviceType::SATA:
				sType="Serial ATA (SATA)";
				break;
			default:
				sType="<NULL>";
				break;
		}
		printf("\tType: %s\n", sType);
	}
}

static void detectControllers()
{
	const auto &p=AtaController::primary();
	const auto &s=AtaController::secondary();

	auto driveType = p.detect(AtaController::Drive::Primary);
	reportController("Primary IDE controller Master drive: %s\n", driveType);
	if(driveType!=AtaController::AtaDeviceType::UNKNOWN)
	{		
		p.identify(AtaController::Drive::Primary);
	}
	driveType = p.detect(AtaController::Drive::Secondary);
	reportController("Primary IDE controller Slave drive: %s\n", driveType);
	if(driveType!=AtaController::AtaDeviceType::UNKNOWN)
	{		
		p.identify(AtaController::Drive::Secondary);
	}


	driveType = s.detect(AtaController::Drive::Primary);
	reportController("Secondary IDE controller Master drive: %s\n", driveType);
	if(driveType!=AtaController::AtaDeviceType::UNKNOWN)
	{		
		s.identify(AtaController::Drive::Primary);
	}
	driveType = p.detect(AtaController::Drive::Secondary);
	reportController("Secondary IDE controller Slave drive: %s\n", driveType);
	if(driveType!=AtaController::AtaDeviceType::UNKNOWN)
	{		
		s.identify(AtaController::Drive::Secondary);
	}
}

static void accessP4Table()
{
	const uint64_t canonicalExtendMask = 0xFFFF000000000000;
	auto vAddrP4Table = reinterpret_cast<const uint64_t *>(canonicalExtendMask | 0x0000FFFFFFFFF000);
	printf("Access Page: p4_table last entry: 0x%0.16lx\n", vAddrP4Table[511]);
}

static void dumpPageTables()
{
	const uint64_t canonicalExtendMask = 0xFFFF000000000000;

	auto vAddrP4Table = reinterpret_cast<const PageTable64<9> *>(canonicalExtendMask | 0x0000FFFFFFFFF000);
	auto vAddrP3Table0 = reinterpret_cast<const PageTable64<9> *>(canonicalExtendMask | 0x0000FFFFFFE00000);
	auto vAddrP2Table0 = reinterpret_cast<const PageTable64<9> *>(canonicalExtendMask | 0x0000FFFFC0000000);
	auto vAddrP1Table0 = reinterpret_cast<const PageTable64<9> *>(canonicalExtendMask | 0x0000FF8000000000);
	
	for( auto i=0; i<PageTable64<9>::NUM_PAGES; ++i)
	{
		if( (*vAddrP4Table)[i] != 0 )
		{
			printf("P4Table: Found non-zero entry: 0x%lx\n", (*vAddrP4Table)[i]);
		}
	}

	for( auto i=0; i<PageTable64<9>::NUM_PAGES; ++i)
	{
		if( (*vAddrP3Table0)[i] != 0 )
		{
			printf("P3Table: Found non-zero entry: 0x%lx\n", (*vAddrP3Table0)[i]);
		}
	}

	for( auto i=0; i<PageTable64<9>::NUM_PAGES; ++i)
	{
		if( (*vAddrP2Table0)[i] != 0 )
		{
			printf("P2Table: Found non-zero entry: 0x%lx\n", (*vAddrP2Table0)[i]);
		}
	}

	for( auto i=0; i<PageTable64<9>::NUM_PAGES; ++i)
	{
		if( (*vAddrP1Table0)[i] != 0 )
		{
			printf("P1Table: Found non-zero entry: %lx\n", (*vAddrP1Table0)[i]);
		}
	}

}

uint64_t *PML4EEntryVAddr(size_t index)
{
	constexpr uint64_t recursiveAddr = 0xFFFFFFFFFFFFF000;
	if( index < 512 )
	{
		return reinterpret_cast<uint64_t *>(recursiveAddr + index);
	}
	return nullptr;
}

uint64_t *PDPTEEntryVAddr(size_t plme4Index, size_t pdpteIndex)
{
	const auto *plme4Entry = PML4EEntryVAddr(plme4Index);
	if( plme4Entry && pdpteIndex < 512)
	{
		constexpr uint64_t recursiveAddr = 0xFFFFFFFFFFE00000;
		return reinterpret_cast<uint64_t *>((recursiveAddr | (plme4Index << 12)) + pdpteIndex);
	}
	return nullptr;
}

uint64_t *PDEEEntryVAddr(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex)
{
	const auto *pdpteEntry = PDPTEEntryVAddr(plme4Index, pdpteIndex);
	if( pdpteEntry && pdeIndex < 512 )
	{
		constexpr uint64_t recursiveAddr = 0xFFFFFFFFC0000000;
		return reinterpret_cast<uint64_t *>((recursiveAddr | (plme4Index << 21) | (pdpteIndex<<12) ) + pdeIndex);
	}
	return nullptr;
}

uint64_t *PTEEntryVAddr(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex)
{
	const auto *pdeEntry = PDEEEntryVAddr(plme4Index, pdpteIndex, pdeIndex);
	if( pdeEntry && pteIndex < 512 )
	{
		constexpr uint64_t recursiveAddr = 0xFFFFFFFFC0000000;
		return reinterpret_cast<uint64_t *>((recursiveAddr | (plme4Index << 30) | (pdpteIndex<<21) | (pdeIndex<<12)) + pteIndex);
	}
	return nullptr;
}

uint64_t getPML4EEntry(size_t index)
{
	const auto *vAddr = PML4EEntryVAddr(index);
	return vAddr ? *vAddr : 0u;
}

uint64_t getPDPTEEntry(size_t plme4Index, size_t pdpteIndex)
{
	const auto *vAddr = PDPTEEntryVAddr(plme4Index, pdpteIndex);
	return vAddr ? *vAddr : 0u;
}

uint64_t getPDEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex)
{
	const auto *vAddr = PDEEEntryVAddr(plme4Index, pdpteIndex, pdeIndex);
	return vAddr ? *vAddr : 0u;
}

uint64_t getPTEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex)
{
	const auto *vAddr = PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, pteIndex);
	return vAddr ? *vAddr : 0u;
}

bool setPML4EEntry(size_t index, uint64_t val)
{
	auto *vAddr = PML4EEntryVAddr(index);
	if( vAddr )
	{
		*vAddr = val;
	}
	return (vAddr != nullptr);
}

bool setPDPTEEntry(size_t plme4Index, size_t pdpteIndex, uint64_t val)
{
	auto *vAddr = PDPTEEntryVAddr(plme4Index, pdpteIndex);
	if( vAddr )
	{
		*vAddr = val;
	}
	return (vAddr != nullptr);
}

bool setPDEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, uint64_t val)
{
	auto *vAddr = PDEEEntryVAddr(plme4Index, pdpteIndex, pdeIndex);
	if( vAddr )
	{
		*vAddr = val;
	}
	return (vAddr != nullptr);
}

bool setPTEEntry(size_t plme4Index, size_t pdpteIndex, size_t pdeIndex, size_t pteIndex, uint64_t val)
{
	auto *vAddr = PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, pteIndex);
	if( vAddr )
	{
		*vAddr = val;
	}
	return (vAddr != nullptr);
}

uint64_t *lookupPTEEntry(uint64_t vAddr)
{
	size_t plme4Index = ((vAddr >> 39) & 0x00000000000001FFu);
	size_t pdpteIndex = ((vAddr >> 30) & 0x00000000000001FFu);
	size_t pdeIndex = ((vAddr >> 21) & 0x00000000000001FFu);
	size_t pteIndex = ((vAddr >> 12) & 0x00000000000001FFu);
	return PTEEntryVAddr(plme4Index, pdpteIndex, pdeIndex, pteIndex);
}

bool createPTEEntry(uint64_t vAddr)
{
	/*
	size_t plme4Index = ((vAddr >> 39) & 0x00000000000001FFu);
	size_t pdpteIndex = ((vAddr >> 30) & 0x00000000000001FFu);
	size_t pdeIndex = ((vAddr >> 21) & 0x00000000000001FFu);
	size_t pteIndex = ((vAddr >> 12) & 0x00000000000001FFu);
	
	auto entry = getPML4EEntry(plme4Index);
	if( entry == 0 )
	{
		entry = allocFrame();
		setPML4EEntry(plme4Index, entry);
	}
	entry = getPDPTEEntry(plme4Index, pdpteIndex);
	if( entry == 0 )
	{
		entry = allocFrame();
		setPDPTEEntry(plme4Index, pdpteIndex, entry);
	}
	entry = getPDEEntry(plme4Index, pdpteIndex, pdeIndex);
	if( entry == 0 )
	{
		entry = allocFrame();
		setPDEEntry(plme4Index, pdpteIndex, pdeIndex, entry);
	}
	entry = getPTEEntry(plme4Index, pdpteIndex, pdeIndex, pteIndex);
	if( entry == 0 )
	{
		entry = allocFrame();
		setPTEEntry(plme4Index, pdpteIndex, pdeIndex, pteIndex, entry);
	}
	return true;
	*/
	return false;
}

void testVmmPageStack(const MultiBootInfoHeader *mboot_header)
{
	const auto *tag = findMultiBootInfoHeaderTag(mboot_header, MULTIBOOT_TAG_TYPE_MMAP);
	PhysicalMemoryBlock *pHead = nullptr;
	if( tag )
	{
		const auto *mmapStart = reinterpret_cast<const multiboot_tag_mmap *>(tag);
		for( auto *mmap = mmapStart->entries;
			 reinterpret_cast<const multiboot_uint8_t *>(mmap) < reinterpret_cast<const multiboot_uint8_t *>(mmapStart) + mmapStart->size;
			 mmap = reinterpret_cast<const multiboot_memory_map_t *> (reinterpret_cast<const uint8_t *>(mmap) + mmapStart->entry_size) )
		{
			if( mmap->type == MULTIBOOT_MEMORY_AVAILABLE && mmap->addr > 0x0000000000100000)
			{
				auto *newBlock = New<PhysicalMemoryBlock>();
				newBlock->start = ((mmap->addr) >> 12);
				newBlock->len = ((mmap->len) >> 12);
				if( !pHead )
				{
					pHead = newBlock;
				}
				else
				{
					auto *ptr = pHead;
					while( ptr->next != nullptr )
					{
						ptr = ptr->next;
					}
					ptr->next = newBlock;
				}
			}
		}
	}
	if( pHead )
	{
		VmmPageStack stack{pHead};
		auto p1 = stack.allocPage();
		auto p2 = stack.allocPage();
		auto p3 = stack.allocPage();
		auto p4 = stack.allocPage();
		auto p5 = stack.allocPage();

		stack.freePage(p5);
		stack.freePage(p1);
		stack.freePage(p3);
		p5 = stack.allocPage();
		stack.freePage(p2);
		stack.freePage(p4);
		stack.freePage(p5);
	}
	// Incomming change from dan/msvc
	// PTE_64_4K *pte = reinterpret_cast<PTE_64_4K *>(0xfffffffffffff000);
	// printf("pte->physical[511] == 0x%016.16lx\n", pte->physical[511]);
	// pte->physical[511] |= (1<<8);
	// // make the recursive entry in plme4 global
}