#if 0

#include "multiboot.h"

#define MBOOT_PAGE_ALIGN    1
#define MBOOT_MEM_INFO      2 
#define MBOOT_HEADER_MAGIC  0x1BADB002u  // ; Multiboot Magic value
/* NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
// pass us a symbol table. */

#define MBOOT_HEADER_FLAGS  ( MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO )
#define MBOOT_CHECKSUM      ( -( MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS ) )

#ifdef __cplusplus
extern "C" {
#endif

extern void mboot(void);
extern void code(void);
extern void bss(void);
extern void end(void);
extern void start(void);

struct multiboot_header HEADER = {
	MBOOT_HEADER_MAGIC,
	MBOOT_HEADER_FLAGS,
	MBOOT_CHECKSUM,
	(multiboot_uint32_t)mboot,
	(multiboot_uint32_t)code,
	(multiboot_uint32_t)bss,
	(multiboot_uint32_t)end,
	(multiboot_uint32_t)start
};

#ifdef __cplusplus
}
#endif

#endif // 0
