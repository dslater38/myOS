#ifndef BOOTINFORMATION_H_INCLUDED__
#define BOOTINFORMATION_H_INCLUDED__

#include "multiboot2.h"

struct BootInformation
{
	const multiboot_tag_framebuffer *frame_buffer{nullptr};
	const multiboot_tag_mmap *mmap{nullptr};
	const multiboot_tag_bootdev	*boot_device{nullptr};
	const multiboot_tag_basic_meminfo	*basic_meminfo{nullptr};
	const multiboot_tag_module *module_info{nullptr};
	const multiboot_tag_string *bootloader_name{nullptr};
	const multiboot_tag_string *command_line{nullptr};
	const multiboot_tag_load_base_addr *load_base_addr{nullptr};
	const multiboot_tag_elf_sections *elf_sections{nullptr};
	const multiboot_tag_apm *apm{nullptr};
	const multiboot_tag_old_acpi *acpi_old{nullptr};
	const multiboot_tag_new_acpi *acpi_new{nullptr};
};

#endif // BOOTINFORMATION_H_INCLUDED__