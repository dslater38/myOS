#ifndef DESCRIPTOR_TABLES_H_INCLUDED
#define DESCRIPTOR_TABLES_H_INCLUDED

#include "types.h"

// This structure contains the value of one GDT entry.
// We use the attribute 'packed' to tell GCC not to change
// any of the alignment in the structure.


const u8int PRESENT_BIT = (1<<7);
const u8int CODE_SEGMENT_BIT = (1<<4);
const u8int DATA_SEGMENT_BIT = (0<<4);
const u8int SYSTEM_SEGMENT_BIT = (1<<4);
const u8int EXECUTABLE_SEGMENT_BIT = (1<<3);
const u8int DATA_SEGMENT_EXPAND_DOWN_BIT = (1<<2);
const u8int CODE_SEGMENT_CONFORMING_BIT = (1<<2);
const u8int SEGMENT_RW_BIT = (1<<1);
const u8int SEGMENT_ACCESSED_BIT = (1<<0);

const u8int GRAN_1K_BIT = (1<<7);
const u8int GRAN_1B_BIT = (0<<7);
const u8int SELECTOR_SIZE_32 = (1<<6);
const u8int SELECTOR_SIZE_16 = (0<<6);
const u8int SELECTOR_SIZE_64 = (0<<6);




const u8int SEGMENT_PRESENT = (1<<0);
const u8int SYSTEM_DESCRIPTOR = (1<<1);
const u8int GRAN_1K = (1<<2);
const u8int OPERAND_SIZE_32 = (1<<3);
const u8int IS_64BITS = (1<<4);

const u8int SEGMENT_TYPE_CODE = (1u<<3);
const u8int SEGMENT_TYPE_DATA = (0u<<3);

const u8int RING0	=	0u;
const u8int RING1	=	1u;
const u8int RING2	=	2u;
const u8int RING3	=	3u;

const u8int SEGMENT_EXPAND_DOWN = (1u<<2);
const u8int SEGMENT_WRITE_ENABLED = (1u<<1);
const u8int SEGMENT_EXECUTE_ENABLED = (1u<<1);
const u8int SEGMENT_ACCESSED = (1u<<0);

struct gdt_entry_t
{
	u16int &limit_low() noexcept	{ return *(u16int *)data; }
	u16int limit_low()const noexcept	{ return *(u16int *)data; }
	
	u16int &base_low() noexcept		{ return *(u16int *)(data+2); };
	u16int base_low()const noexcept		{ return *(u16int *)(data+2); };

	u8int &base_middle()noexcept	{ return *(data+4); };
	u8int base_middle()const noexcept	{ return *(data+4); };

	u8int &access() noexcept		{ return *(data+5); };
	u8int access()const noexcept		{ return *(data+5); };

	u8int &granularity() noexcept	{ return *(data+6); };
	u8int granularity()const noexcept	{ return *(data+6); };

	u8int &base_high() noexcept		{ return *(data+7); };
	u8int base_high()const noexcept		{ return *(data+7); };

	void set(u32int base, u32int limit, u8int type, u8int privlege_level, u8int flags)
	{
		limit_low()   = static_cast<u16int>(limit & 0xFFFF);
		base_low()    = static_cast<u16int>(base & 0xFFFF);
		base_high()   = static_cast<u8int>((base >> 24) & 0xFF);
		
		u8int acc =  (type & 0x0F);
		
		if( 0 == (flags & SYSTEM_DESCRIPTOR))
		{
			acc |= static_cast<u8int>(1<<4);
		}
		
		acc |= static_cast<u8int>((privlege_level & 0x03)<<5);
		
		if(flags & SEGMENT_PRESENT)
		{
			acc |= static_cast<u8int>(1<<7);
		}
		access() = acc;
		
		u8int gran = static_cast<u8int>((limit>>16) & 0x0F);
		if(type & IS_64BITS)
		{
			gran |= (1<<5);
		}
		if(flags & OPERAND_SIZE_32)
		{
			gran |= (1<<6);
		}

		if(flags & GRAN_1K)
		{
			gran |= (1<<7);
		}
		granularity() = gran;
		base_high() = static_cast<u8int>((base >> 24) & 0xFF);
	}
	
	void set(u32int base, u32int limit, u8int access_, u8int gran)noexcept
	{
		base_low()    = (base & 0xFFFF);
		base_middle() = (base >> 16) & 0xFF;
		base_high()   = (base >> 24) & 0xFF;

		limit_low()   = (limit & 0xFFFF);
		granularity() = (limit >> 16) & 0x0F;

		granularity() |= gran & 0xF0;
		access()      = access_;		
	}

	u8int data[8];
};

struct gdt_ptr_t
{

	u16int &limit() noexcept{ return *(u16int *)data;}
	u16int limit()const noexcept{ return *(u16int *)data;}
	u32int &base() noexcept{ return *(u32int *)(data+2);}
	u32int base()const noexcept{ return *(u32int *)(data+2);}


	u8int data[6];
};


struct gdt64_ptr_t
{

	u16int &limit() noexcept{ return *(u16int *)data;}
	u16int limit()const noexcept{ return *(u16int *)data;}
	u64int &base() noexcept{ return *(u64int *)(data+2);}
	u64int base()const noexcept{ return *(u64int *)(data+2);}


	u8int data[10];
};


struct idt_entry_t
{
	u16int &base_lo()noexcept { return *(u16int *)(data);}
	u16int base_lo() const noexcept { return *(u16int *)(data);}

	u16int &sel()noexcept { return *(u16int *)(data+2); }
	u16int sel() const noexcept { return *(u16int *)(data+2); }

	u8int  &always0()noexcept { return *(data+4); }
	u8int  always0() const noexcept { return *(data+4); }

	u8int  &flags()noexcept { return *(data+5); }
	u8int  flags() const noexcept { return *(data+5); }

	u16int &base_hi()noexcept { return *(u16int *)(data+6); }
	u16int base_hi() const noexcept { return *(u16int *)(data+6); }

	void set(u32int base, u16int sel, u8int flags)
	{
		this->base_lo() = base & 0xFFFF;
		this->base_hi() = (base >> 16) & 0xFFFF;

		this->sel()     = sel;
		this->always0() = 0;
		// We must uncomment the OR below when we get to using user-mode.
		// It sets the interrupt gate's privilege level to 3.
		this->flags()   = flags /* | 0x60 */;

	}

	u8int data[8];
};

struct idt_ptr_t
{
	u16int &limit()noexcept { return *(u16int *)(data);}
	u16int limit()const noexcept { return *(u16int *)(data);}

	u32int &base()noexcept {return *(u32int *)(data + 2);}                // The address of the first element in our idt_entry_t array.
	u32int base()const noexcept {return *(u32int *)(data + 2);}
	u8int data[6];
};



struct idt_entry64_t
{
	u16int &base_lo()noexcept { return *(u16int *)(data);}
	u16int base_lo() const noexcept { return *(u16int *)(data);}

	u16int &sel()noexcept { return *(u16int *)(data+2); }
	u16int sel() const noexcept { return *(u16int *)(data+2); }

	u8int  &always0()noexcept { return *(data+4); }
	u8int  always0() const noexcept { return *(data+4); }

	u8int  &flags()noexcept { return *(data+5); }
	u8int  flags() const noexcept { return *(data+5); }

	u16int &base_mid()noexcept { return *(u16int *)(data+6); }
	u16int base_mid() const noexcept { return *(u16int *)(data+6); }
	
	u32int &base_hi()noexcept { return *(u32int *)(data+8); }
	u32int base_hi()const noexcept { return *(u32int *)(data+8); }
	
	u32int &reserved()noexcept { return *(u32int *)(data+12); }
	u32int reserved()const noexcept { return *(u32int *)(data+12); }

	void set(u64int base, u16int sel, u8int flags)
	{
		this->base_lo() = (u16int)(base & 0xFFFF);
		this->base_mid() = (u16int)((base >> 16) & 0xFFFF);

		this->sel()     = sel;
		this->always0() = 0;
		// We must uncomment the OR below when we get to using user-mode.
		// It sets the interrupt gate's privilege level to 3.
		this->flags()   = flags /* | 0x60 */;
		
		this->base_hi() = (u32int)( (base >> 32) & 0xFFFFFFFF );
		this->reserved() = 0;
	}

	u8int data[16];
};

struct idt_ptr64_t
{
	u16int &limit()noexcept { return *(u16int *)(data);}
	u16int limit()const noexcept { return *(u16int *)(data);}

	u64int &base()noexcept {return *(u64int *)(data + 2);}                // The address of the first element in our idt_entry_t array.
	u64int base()const noexcept {return *(u64int *)(data + 2);}
	u8int data[10];
};

#if 0

struct gdt_entry_struct
{
	u16int limit_low;           // The lower 16 bits of the limit.
	u16int base_low;            // The lower 16 bits of the base.
	u8int  base_middle;         // The next 8 bits of the base.
	u8int  access;              // Access flags, determine what ring this segment can be used in.
	u8int  granularity;
	u8int  base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
	u16int limit;               // The upper 16 bits of all selector limits.
	u32int base;                // The address of the first gdt_entry_t struct.
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;


// A struct describing an interrupt gate.
struct idt_entry_struct
{
	u16int base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
	u16int sel;                 // Kernel segment selector.
	u8int  always0;             // This must always be zero.
	u8int  flags;               // More flags. See documentation.
	u16int base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct idt_ptr_struct
{
	u16int limit;
	u32int base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// These extern directives let us access the addresses of our ASM ISR handlers.

#endif // 0

#ifdef __cplusplus
extern "C" {
#endif

// Initialisation function is publicly accessible.
void init_descriptor_tables();

extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();


extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
#ifdef __cplusplus
}
#endif


#endif // DESCRIPTOR_TABLES_H_INCLUDED
