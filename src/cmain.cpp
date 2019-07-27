/*  kernel.c - the C part of the kernel */
/*  Copyright (C) 1999, 2010  Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "multiboot2.h"
#include "vesavga.h"
#include "kmalloc.h"
#include <assert.h>

#ifndef __x86_64__

#include <elf/elf.h>

#endif

/*  Macros. */

/*  Some screen stuff. */
/*  The number of columns. */
#define COLUMNS                 80
/*  The number of lines. */
#define LINES                   24
/*  The attribute of an character. */
#define ATTRIBUTE               7
/*  The video memory address. */
#define VIDEO                   0xB8000

// #define HIDWORD(a) ((unsigned)(a >> 32))
// #define LODWORD(a) ((unsigned)(a & 0xFFFFFFFF))

/*  Variables. */
/*  Save the X position. */
//static int xpos;
/*  Save the Y position. */
//static int ypos;
/*  Point to the video memory. */
static volatile unsigned char *video;

/*  Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
static void cls (void);
//static void itoa (char *buf, int base, int d);
//static void putchar (int c);
int printf (const char *format, ...);

#define MMAP_RAM 1
#define MMAP_ACPI 3
#define MMAP_DEFECTIVE 5

static
const char *mmap_type(int n)
{
	switch(n)
	{
		case MMAP_RAM:
			return "RAM";
		case MMAP_ACPI:
			return "acpi";
//        case 4:
//            return "reserved";
		case MMAP_DEFECTIVE:
			return "defective";
		default:
			break;
	}
	return "reserved";
}

void
print_mem_size(uint64_t size, const char *type)
{
	if( size < 10*1024 )
	{
		printf("%u %s", (uint32_t)size, type);
	}
	else if( size < 10*1024*1024 )
	{
		printf("%uK %s", (uint32_t)(size/1024), type);
	}
	else if( size < 10ull*1024ull*1024ull*1024ull )
	{
		printf("%uM %s", (uint32_t)(size/(1024*1024)), type);
	}
	else if( size < 10ull*1024ull*1024ull*1024ull*1024ull )
	{
		printf("%uG %s", (uint32_t)(size/(1024ull*1024ull*1024ull)), type);
	}
	else
	{
		printf("%uT %s", (uint32_t)(size/(1024ull*1024ull*1024ull*1024ull)), type);
	}
}
#if 0
/* ELF standard typedefs (yet more proof that <stdint.h> was way overdue) */
typedef uint16_t Elf64_Half;
typedef int16_t Elf64_SHalf;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Section;

/* Lenght of magic at the start of a file */
#define EI_NIDENT	16

struct Elf64_Ehdr {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
};

/* 
 * Section header
*/
struct Elf64_Shdr {
	uint32_t        sh_name;
	uint32_t        sh_type;
	uint64_t        sh_flags;
	uint64_t        sh_addr;
	uint64_t        sh_offset;
	uint64_t        sh_size;
	uint32_t        sh_link;
	uint32_t        sh_info;
	uint64_t        sh_addralign;
	uint64_t        sh_entsize;
};

/*
 * Program header 
 */
struct Elf64_Phdr {
	uint32_t        p_type;	/* Segment type: Loadable segment = 1 */
	uint32_t        p_flags;	/* Flags: logical "or" of PF_
					 * constants below */
	uint64_t        p_offset;	/* Offset of segment in file */
	uint64_t        p_vaddr;	/* Reqd virtual address of segment 
					 * when loading */
	uint64_t        p_paddr;	/* Reqd physical address of
					 * segment */
	uint64_t        p_filesz;	/* How many bytes this segment
					 * occupies in file */
	uint64_t        p_memsz;	/* How many bytes this segment
					 * should occupy in * memory (when 
					 * * loading, expand the segment
					 * by * concatenating enough zero
					 * bytes to it) */
	uint64_t        p_align;	/* Reqd alignment of segment in
					 * memory */
};

#endif // 0

#ifndef __x86_64__

void *x64entry = 0;

extern "C"
{

	static errval_t elfAlloc(void * /* state */, genvaddr_t /* base */, size_t size, uint32_t flags, void **ret)
	{
		*ret = reinterpret_cast<void *>(kmalloc(size));
		return 0;
	}
	void *load_elf64_module(Elf64_Ehdr *elfFile, size_t size)
	{
		genvaddr_t entry = 0;
		genvaddr_t tlsbase = 0;
		size_t tlsinitlen = 0;
		size_t tlstotallen = 0;
		auto success = elf64_load(EM_X86_64, elfAlloc, nullptr, (lvaddr_t)elfFile, size, &entry, &tlsbase, &tlsinitlen, &tlstotallen);
		// auto success = elf_loadFile(elfFile, 1);
		// if( success )
		// {
		// 	entry = reinterpret_cast<void *>(elf64_getEntryPoint(elfFile));
		// }
		// else
		if( success )
		{
			monitor_write("elf_loadFile FAILED!!!!!!!");
		}
		return reinterpret_cast<void *>(entry);
	}
//~ void *load_elf64_module(Elf64_Ehdr *elfFile)
//~ {
	//~ auto entryPoint = elf64_getEntryPoint(elfFile);
	//~ auto *segments = elf64_getProgramSegmentTable(elfFile);
	//~ auto numSegments = elf64_getNumProgramHeaders(elfFile);
	//~ auto sections = elf64_getSectionTable(elfFile);
	//~ auto numSections = elf64_getNumSections(elfFile);
	//~ for (auto i = 0; i < numSegments; i++)
	//~ {
		//~ const auto &seg = segments[i];
		//~ if (seg.p_type != 1) {
			//~ printf("segment %d is not loadable, skipping\n", i);
		//~ } else {
			//~ const auto *src = reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(elfFile) + seg.p_offset);
			//~ memcpy( reinterpret_cast<void *>(static_cast<uintptr_t>(seg.p_paddr)), src, seg.p_filesz );
		//~ }
	//~ }
	//~ return reinterpret_cast<void *>(entryPoint);
//~ }

}

#endif // #ifndef __x86_64__


static
void
processModule(uint32_t start, uint32_t end)
{
#ifndef __x86_64__
	Elf64_Ehdr *pElf64 = reinterpret_cast<Elf64_Ehdr *>(static_cast<uintptr_t>(start));
	
#if 0	
	printf("Elf64: e_ident: %16s\n", pElf64->e_ident);
	printf("Elf64: type: %x\n", pElf64->e_type);
	printf("Elf64: machine: %x\n", pElf64->e_machine);
	printf("Elf64: version: %x\n", pElf64->e_version);	
	printf("Elf64: entry: 0x%08.8x%08.8x\n", HIDWORD(pElf64->e_entry),LODWORD(pElf64->e_entry));
	printf("Elf64: phoffset: 0x%08.8x%08.8x\n", HIDWORD(pElf64->e_phoff),LODWORD(pElf64->e_phoff));
	printf("Elf64: shoffset: 0x%08.8x%08.8x\n", HIDWORD(pElf64->e_shoff),LODWORD(pElf64->e_shoff));
	printf("Elf64: flags: %x\n", pElf64->e_flags);	
	printf("Elf64: ehsize: %x\n", pElf64->e_ehsize);	
	printf("Elf64: phentsize: %x\n", pElf64->e_phentsize);	
	printf("Elf64: phnum: %x\n", pElf64->e_phnum);	
	printf("Elf64: shentsize: %x\n", pElf64->e_shentsize);	
	printf("Elf64: shnum: %x\n", pElf64->e_shnum);	
	printf("Elf64: shstrndx: %x\n", pElf64->e_shstrndx);	
	
	
	
	
	Elf64_Phdr *phdr = reinterpret_cast<Elf64_Phdr *>(pElf64->e_phoff + reinterpret_cast<uint8_t *>(start));
	printf("PHDR: type: 0x%0x\n", phdr->p_type);
	printf("PHDR: flags: %x\n", phdr->p_flags);
	
	printf("PHDR: offset: 0x%08.8x%08.8x\n", HIDWORD(phdr->p_offset), LODWORD(phdr->p_offset));
	printf("PHDR: vaddr: 0x%08.8x%08.8x\n", HIDWORD(phdr->p_vaddr), LODWORD(phdr->p_vaddr));
	printf("PHDR: paddr: 0x%08.8x%08.8x\n", HIDWORD(phdr->p_paddr), LODWORD(phdr->p_paddr));
	
	printf("PHDR: filesize: 0x%08.8x%08.8x\n", HIDWORD(phdr->p_filesz), LODWORD(phdr->p_filesz));
	printf("PHDR: memsize: 0x%08.8x%08.8x\n", HIDWORD(phdr->p_memsz), LODWORD(phdr->p_memsz));
	printf("PHDR: align: 0x%08.8x%08.8x\n", HIDWORD(phdr->p_align), LODWORD(phdr->p_align));
	#endif // 0
	
// 	elf64_showDetails(pElf64, pElf64->e_shoff, "kernel" );
	x64entry = load_elf64_module(pElf64, end-start);
#endif // __x86_64__
}


/*  Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
cmain (unsigned long magic, unsigned long addr)
{  
	struct multiboot_tag *tag;
	unsigned size;

	/*  Clear the screen. */
//	cls (); 

	/*  Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
	  printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
	  return;
	}

	if (addr & 7)
	{
	  printf ("Unaligned mbi: 0x%lx\n", addr);
	  return;
	}

	  size = *(unsigned *) addr;
	  printf ("Announced mbi size %d (%08.8x) bytes\n", size, size);
	  for (tag = (struct multiboot_tag *) (addr + 8);
		   tag->type != MULTIBOOT_TAG_TYPE_END ;
		   tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										   + ((tag->size + 7) & ~7)))
	{
		uint8_t oldColor = set_foreground_color(RED);
		printf ("Tag 0x%08.8x, Size 0x%08.8x\n", tag->type, tag->size);
		if( oldColor != 0xFF )
		{
			set_foreground_color(oldColor);
		}
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
			{
				struct multiboot_tag_elf_sections *elf = (struct multiboot_tag_elf_sections *)tag;
				printf("elf sections, num %d, entsize %d, shndx: %d\n", elf->num, elf->entsize, elf->shndx);
				break;
			}
			case MULTIBOOT_TAG_TYPE_APM:
			{
				struct multiboot_tag_apm *apm = (struct multiboot_tag_apm *)tag;
				printf("apm\n");
				printf("Version: %d, cseg 0x%04.4x, offset 0x%08.8x\n", apm->version, apm->cseg, apm->offset);
				printf("cseg16: 0x%04.4x, dseg: 0x%04.4x, flags: 0x%04.4x\n", apm->cseg_16, apm->dseg, apm->flags );
				printf("cseg_len: %d, cseg_16_len: %d, dseg_len: %d\n", apm->cseg_len, apm->cseg_16_len, apm->dseg_len);
				break;
			}
			case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
			{
				struct multiboot_tag_load_base_addr *info = (struct multiboot_tag_load_base_addr *)tag;
				printf("load base addr: 0x%08.8x\n", info->load_base_addr);
				break;
			}
			case MULTIBOOT_TAG_TYPE_ACPI_OLD:
			{
				struct multiboot_tag_old_acpi *acpi = (struct multiboot_tag_old_acpi *)(tag);
				unsigned char *ptr = acpi->rsdp;
				printf("acpi old\n");
				printf("Signature: %.8s, ", (char *)ptr );
				ptr += 8;
				printf("checksum: %d, ", (unsigned)(*ptr));
				ptr++;
				printf("OEMID: %.6s,\n", (char *)ptr );
				ptr += 6;
				printf("revision: %d, ", (unsigned)(*ptr));
				++ptr;
				printf("RstAddress: 0x%08.8x\n", *((multiboot_uint32_t *)ptr) );
				break;
			}
			case MULTIBOOT_TAG_TYPE_ACPI_NEW:
			{
				struct multiboot_tag_new_acpi *acpi = (struct multiboot_tag_new_acpi *)(tag);
				unsigned char *ptr = acpi->rsdp;
				printf("acpi new\n");
				printf("Signature: %.8s, ", (char *)ptr );
				ptr += 8;
				printf("checksum: %d, ", (unsigned)(*ptr));
				ptr++;
				printf("OEMID: %.6s,\n", (char *)ptr );
				ptr += 6;
				printf("revision: %d, ", (unsigned)(*ptr));
				++ptr;
				printf("RstAddress: 0x%08.8x\n", *((multiboot_uint32_t *)ptr) );
				ptr += 4;
				printf("Length: %d\t", *((multiboot_uint32_t *)ptr) );
				ptr += 4;
				printf("XsdtAddress 0x08.8%x08.8%x\t", *((multiboot_uint32_t *)ptr+4), *((multiboot_uint32_t *)ptr) );
				ptr += 8;
				printf("reserved: {%d, %d, %d}\n", (unsigned)ptr[0], (unsigned)ptr[1], (unsigned)ptr[2]);
				break;
			}

		case MULTIBOOT_TAG_TYPE_CMDLINE:
		  printf ("Command line = %s\n",
				  ((struct multiboot_tag_string *) tag)->string);
		  break;
		case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
		  printf ("Boot loader name = %s\n",
				  ((struct multiboot_tag_string *) tag)->string);
		  break;
		case MULTIBOOT_TAG_TYPE_MODULE:
		  printf ("Module at 0x%08.8x-0x%08.8x. Command line %s\n",
				  ((struct multiboot_tag_module *) tag)->mod_start,
				  ((struct multiboot_tag_module *) tag)->mod_end,
				  ((struct multiboot_tag_module *) tag)->cmdline);
		processModule(((struct multiboot_tag_module *) tag)->mod_start, ((struct multiboot_tag_module *) tag)->mod_end);
		  break;
		case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
		  printf ("mem_lower = %uKB, (0x%08.8xKB) mem_upper = %uKB (0x%08.8xKB)\n",
				  ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
		  ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower,
				  ((struct multiboot_tag_basic_meminfo *) tag)->mem_upper,
		  ((struct multiboot_tag_basic_meminfo *) tag)->mem_upper);
		  break;
		case MULTIBOOT_TAG_TYPE_BOOTDEV:
		  printf ("Boot device 0x%08.8x, %u (0x%08.8x), %u (0x%08.8x)\n",
				  ((struct multiboot_tag_bootdev *) tag)->biosdev,
				  ((struct multiboot_tag_bootdev *) tag)->slice,
		  ((struct multiboot_tag_bootdev *) tag)->slice,
				  ((struct multiboot_tag_bootdev *) tag)->part,
			  ((struct multiboot_tag_bootdev *) tag)->part);
		  break;
		case MULTIBOOT_TAG_TYPE_MMAP:
		  {
			multiboot_memory_map_t *mmap;

		uint64_t	ram_size = 0;
			uint64_t	acpi_size = 0;
		uint64_t	reserved_size = 0;
		uint64_t	defective_size = 0;
		  
			printf ("mmap\n");
	  
			for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				 (multiboot_uint8_t *) mmap 
				   < (multiboot_uint8_t *) tag + tag->size;
				 mmap = (multiboot_memory_map_t *) 
				   ((unsigned long) mmap
					+ ((struct multiboot_tag_mmap *) tag)->entry_size)) {
			  printf (" base_addr = 0x%08.8x%08.8x,"
				  " length = 0x%08.8x%08.8x, type = %s\n",
				HIDWORD(mmap->addr), LODWORD(mmap->addr) , 
				HIDWORD(mmap->len), LODWORD(mmap->len),
				reinterpret_cast<char *>(mmap->type) );
				
			  /* printf (" base_addr = 0x%08.8x%08.8x,"
				  " length = 0x%08.8x%08.8x, type = %s\n",
				  HIDWORD(mmap->addr ),
				  LODWORD(mmap->addr),
				  HIDWORD (mmap->len),
				  LODWORD (mmap->len),
				  mmap_type( mmap->type) ) ; */
				  
				  if( MMAP_RAM == mmap->type)
				  {
					  ram_size += mmap->len;
				  }
				  else if( MMAP_ACPI == mmap->type)
				  {
					  acpi_size += mmap->len;
				  }
				  else if( MMAP_DEFECTIVE == mmap->type)
				  {
					  defective_size += mmap->len;
				  }
				  else
				  {
					  reserved_size += mmap->len;
				  }
		  }
		  printf("Totals: ");
		  print_mem_size(ram_size, "RAM, ");
		  print_mem_size(acpi_size, "ACPI, ");
		  print_mem_size(defective_size, "Defective, ");
		  print_mem_size(reserved_size, "reserved\n");
		  // printf("Totals: %uK RAM, %uK ACPI, %uK Defective, %uK reserved\n", ram_size/1024, acpi_size/1024, defective_size/1024, reserved_size/1024 );
		  }
		  break;
		case MULTIBOOT_TAG_TYPE_VBE:
			{
				multiboot_tag_vbe *vbe = reinterpret_cast<multiboot_tag_vbe *>(tag);
				printf("vbe mode: 0x%x\n", vbe->vbe_mode);
				printf("vbe_interface: seg: 0x%x, off: 0x%x, len: 0x%x\n", vbe->vbe_interface_seg, vbe->vbe_interface_off, vbe->vbe_interface_len);
				printf("vbe_control_info 0x%08.8x, vbe_mode_info: 0x%08.8x\n", vbe->vbe_control_info.external_specification, vbe->vbe_mode_info.external_specification);
				break;
			}
		case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
		  {
			multiboot_uint32_t color;
			unsigned i;
			struct multiboot_tag_framebuffer *tagfb
			  = (struct multiboot_tag_framebuffer *) tag;
			void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;

			printf("framebuffer: type: %d\n", tagfb->common.framebuffer_type);
			printf("addr: 0x%08.8x%08.8x, ", HIDWORD(tagfb->common.framebuffer_addr), LODWORD(tagfb->common.framebuffer_addr));
			printf("pitch: %d, width: %d, height: %d, bpp: %d\n", tagfb->common.framebuffer_pitch,
			tagfb->common.framebuffer_width, tagfb->common.framebuffer_height, tagfb->common.framebuffer_bpp);

			switch (tagfb->common.framebuffer_type)
			  {
			  case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
				{
					printf("Indexed Frame Buffer.\n");
				  unsigned best_distance, distance;
				  struct multiboot_color *palette;
			
				  palette = tagfb->framebuffer_palette;

				  color = 0;
				  best_distance = 4*256*256;
			
				  for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++)
					{
					  distance = (0xff - palette[i].blue) 
						* (0xff - palette[i].blue)
						+ palette[i].red * palette[i].red
						+ palette[i].green * palette[i].green;
					  if (distance < best_distance)
					  if (distance < best_distance)
						{
						  color = i;
						  best_distance = distance;
						}
					}
				}
				break;

			  case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			  	printf("RGB Frame Buffer.\n");
				color = ((1 << tagfb->framebuffer_blue_mask_size) - 1) 
				  << tagfb->framebuffer_blue_field_position;
				break;

			  case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			  	printf("EGA Text Frame Buffer.\n");
				color = '\\' | 0x0100;
				break;

			  default:
			  	printf("Unrecognized Frame Buffer.\n");
				color = 0xffffffff;
				break;
			  }
			
			for (i = 0; i < tagfb->common.framebuffer_width
				   && i < tagfb->common.framebuffer_height; i++)
			  {
				switch (tagfb->common.framebuffer_bpp)
				  {
				  case 8:
					{
					  multiboot_uint8_t *pixel = reinterpret_cast<multiboot_uint8_t *>(fb)
						+ tagfb->common.framebuffer_pitch * i + i;
					  *pixel = color;
					}
					break;
				  case 15:
				  case 16:
					{
					  multiboot_uint16_t *pixel
						= reinterpret_cast<multiboot_uint16_t *>(reinterpret_cast<multiboot_uint8_t *>(fb) + tagfb->common.framebuffer_pitch * i + 2 * i);
					  *pixel = color;
					}
					break;
				  case 24:
					{
					  multiboot_uint32_t *pixel
						= reinterpret_cast<multiboot_uint32_t *>(reinterpret_cast<multiboot_uint8_t *>(fb) + tagfb->common.framebuffer_pitch * i + 3 * i);
					  *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
					}
					break;

				  case 32:
					{
					  multiboot_uint32_t *pixel
						= reinterpret_cast<multiboot_uint32_t *>(reinterpret_cast<multiboot_uint8_t *>(fb) + tagfb->common.framebuffer_pitch * i + 4 * i);
					  *pixel = color;
					}
					break;
				  }
			  }
			break;
		  }
		}
	}
	tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
								  + ((tag->size + 7) & ~7));
	printf ("Total mbi size %u (0x%08.8x)\n", (unsigned) (unsigned long long)(tag - addr), (unsigned)(unsigned long long) (tag - addr));
}    

/*  Clear the screen and initialize VIDEO, XPOS and YPOS. */
static void
cls (void)
{
	extern void monitor_clear(void);
	monitor_clear();
}

extern "C"
{
	void __assert_fail(const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function)
	{
		printf("%s: FILE: %s, LINE: %d, FUNCTION: %s\n", __assertion, __file, __line, __function);
		PANIC("Assertion Failed\n");
	}
}

#if 0

/*  Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
static void
itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /*  If %d is specified and D is minus, put '-' in the head. */
  if (base == 'd' && d < 0)
	{
	  *p++ = '-';
	  buf++;
	  ud = -d;
	}
  else if (base == 'x')
	divisor = 16;

  /*  Divide UD by DIVISOR until UD == 0. */
  do
	{
	  int remainder = ud % divisor;
	  
	  *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}
  while (ud /= divisor);

  /*  Terminate BUF. */
  *p = 0;
  
  /*  Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
	{
	  char tmp = *p1;
	  *p1 = *p2;
	  *p2 = tmp;
	  p1++;
	  p2--;
	}
}

/*  Put the character C on the screen. */
static void
putchar (int c)
{
  if (c == '\n' || c == '\r')
	{
	newline:
	  xpos = 0;
	  ypos++;
	  if (ypos >= LINES)
		ypos = 0;
	  return;
	}

  *(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
  *(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

  xpos++;
  if (xpos >= COLUMNS)
	goto newline;
}
#endif // 0

/*  Format a string and print it on the screen, just like the libc
   function printf. 
void
printf (const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;
  
  while ((c = *format++) != 0)
	{
	  if (c != '%')
		putchar (c);
	  else
		{
		  char *p, *p2;
		  int pad0 = 0, pad = 0;
		  
		  c = *format++;
		  if (c == '0')
			{
			  pad0 = 1;
			  c = *format++;
			}

		  if (c >= '0' && c <= '9')
			{
			  pad = c - '0';
			  c = *format++;
			}

		  switch (c)
			{
			case 'd':
			case 'u':
			case 'x':
			  itoa (buf, c, *((int *) arg++));
			  p = buf;
			  goto string;
			  break;

			case 's':
			  p = *arg++;
			  if (! p)
				p = "(null)";

			string:
			  for (p2 = p; *p2; p2++);
			  for (; p2 < p + pad; p2++)
				putchar (pad0 ? '0' : ' ');
			  while (*p)
				putchar (*p++);
			  break;

			default:
			  putchar (*((int *) arg++));
			  break;
			}
		}
	}
}
*/
