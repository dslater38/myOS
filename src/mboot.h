#ifndef MBOOT_H_INCLUDED
#define MBOOT_H_INCLUDED


typedef struct MBOOTTag
{
	uint32_t	magic;
	uint32_t	flags;
	uint32_t	checksum;
	struct MBOOT *mboot;
	void *code;
	void *bss;
	void *end;
	void (*start)(void);	
}MBOOT;

extern MBOOT *mboot;

#endif // MBOOT_H_INCLUDED
