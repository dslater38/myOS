#ifndef DESCRIPTOR_TABLES_H_INCLUDED
#define DESCRIPTOR_TABLES_H_INCLUDED

#include <stdint.h>

// This structure contains the value of one GDT entry.
// We use the attribute 'packed' to tell GCC not to change
// any of the alignment in the structure.


const uint8_t PRESENT_BIT = (1<<7);
const uint8_t CODE_SEGMENT_BIT = (1<<4);
const uint8_t DATA_SEGMENT_BIT = (0<<4);
const uint8_t SYSTEM_SEGMENT_BIT = (1<<4);
const uint8_t EXECUTABLE_SEGMENT_BIT = (1<<3);
const uint8_t DATA_SEGMENT_EXPAND_DOWN_BIT = (1<<2);
const uint8_t CODE_SEGMENT_CONFORMING_BIT = (1<<2);
const uint8_t SEGMENT_RW_BIT = (1<<1);
const uint8_t SEGMENT_ACCESSED_BIT = (1<<0);

const uint8_t GRAN_1K_BIT = (1<<7);
const uint8_t GRAN_1B_BIT = (0<<7);
const uint8_t SELECTOR_SIZE_32 = (1<<6);
const uint8_t SELECTOR_SIZE_16 = (0<<6);
const uint8_t SELECTOR_SIZE_64 = (0<<6);




const uint8_t SEGMENT_PRESENT = (1<<0);
const uint8_t SYSTEM_DESCRIPTOR = (1<<1);
const uint8_t GRAN_1K = (1<<2);
const uint8_t OPERAND_SIZE_32 = (1<<3);
const uint8_t IS_64BITS = (1<<4);

const uint8_t SEGMENT_TYPE_CODE = (1u<<3);
const uint8_t SEGMENT_TYPE_DATA = (0u<<3);

const uint8_t RING0	=	0u;
const uint8_t RING1	=	1u;
const uint8_t RING2	=	2u;
const uint8_t RING3	=	3u;

const uint8_t SEGMENT_EXPAND_DOWN = (1u<<2);
const uint8_t SEGMENT_WRITE_ENABLED = (1u<<1);
const uint8_t SEGMENT_EXECUTE_ENABLED = (1u<<1);
const uint8_t SEGMENT_ACCESSED = (1u<<0);

struct gdt_entry_t
{
	uint16_t &limit_low() noexcept	{ return *(uint16_t *)data; }
	uint16_t limit_low()const noexcept	{ return *(uint16_t *)data; }
	
	uint16_t &base_low() noexcept		{ return *(uint16_t *)(data+2); };
	uint16_t base_low()const noexcept		{ return *(uint16_t *)(data+2); };

	uint8_t &base_middle()noexcept	{ return *(data+4); };
	uint8_t base_middle()const noexcept	{ return *(data+4); };

	uint8_t &access() noexcept		{ return *(data+5); };
	uint8_t access()const noexcept		{ return *(data+5); };

	uint8_t &granularity() noexcept	{ return *(data+6); };
	uint8_t granularity()const noexcept	{ return *(data+6); };

	uint8_t &base_high() noexcept		{ return *(data+7); };
	uint8_t base_high()const noexcept		{ return *(data+7); };

	void set(uint32_t base, uint32_t limit, uint8_t type, uint8_t privlege_level, uint8_t flags)
	{
		limit_low()   = static_cast<uint16_t>(limit & 0xFFFF);
		base_low()    = static_cast<uint16_t>(base & 0xFFFF);
		base_high()   = static_cast<uint8_t>((base >> 24) & 0xFF);
		
		uint8_t acc =  (type & 0x0F);
		
		if( 0 == (flags & SYSTEM_DESCRIPTOR))
		{
			acc |= static_cast<uint8_t>(1<<4);
		}
		
		acc |= static_cast<uint8_t>((privlege_level & 0x03)<<5);
		
		if(flags & SEGMENT_PRESENT)
		{
			acc |= static_cast<uint8_t>(1<<7);
		}
		access() = acc;
		
		uint8_t gran = static_cast<uint8_t>((limit>>16) & 0x0F);
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
		base_high() = static_cast<uint8_t>((base >> 24) & 0xFF);
	}
	
	void set(uint32_t base, uint32_t limit, uint8_t access_, uint8_t gran)noexcept
	{
		base_low()    = (base & 0xFFFF);
		base_middle() = (base >> 16) & 0xFF;
		base_high()   = (base >> 24) & 0xFF;

		limit_low()   = (limit & 0xFFFF);
		granularity() = (limit >> 16) & 0x0F;

		granularity() |= gran & 0xF0;
		access()      = access_;		
	}

	uint8_t data[8];
};

struct gdt_ptr_t
{

	uint16_t &limit() noexcept{ return *(uint16_t *)data;}
	uint16_t limit()const noexcept{ return *(uint16_t *)data;}
	uint32_t &base() noexcept{ return *(uint32_t *)(data+2);}
	uint32_t base()const noexcept{ return *(uint32_t *)(data+2);}


	uint8_t data[6];
};


struct gdt64_ptr_t
{

	uint16_t &limit() noexcept{ return *(uint16_t *)data;}
	uint16_t limit()const noexcept{ return *(uint16_t *)data;}
	uint64_t &base() noexcept{ return *(uint64_t *)(data+2);}
	uint64_t base()const noexcept{ return *(uint64_t *)(data+2);}


	uint8_t data[10];
};


struct idt_entry_t
{
	uint16_t &base_lo()noexcept { return *(uint16_t *)(data);}
	uint16_t base_lo() const noexcept { return *(uint16_t *)(data);}

	uint16_t &sel()noexcept { return *(uint16_t *)(data+2); }
	uint16_t sel() const noexcept { return *(uint16_t *)(data+2); }

	uint8_t  &always0()noexcept { return *(data+4); }
	uint8_t  always0() const noexcept { return *(data+4); }

	uint8_t  &flags()noexcept { return *(data+5); }
	uint8_t  flags() const noexcept { return *(data+5); }

	uint16_t &base_hi()noexcept { return *(uint16_t *)(data+6); }
	uint16_t base_hi() const noexcept { return *(uint16_t *)(data+6); }

	void set(uint32_t base, uint16_t sel, uint8_t flags)
	{
		this->base_lo() = base & 0xFFFF;
		this->base_hi() = (base >> 16) & 0xFFFF;

		this->sel()     = sel;
		this->always0() = 0;
		// We must uncomment the OR below when we get to using user-mode.
		// It sets the interrupt gate's privilege level to 3.
		this->flags()   = flags /* | 0x60 */;

	}

	uint8_t data[8];
};

struct idt_ptr_t
{
	uint16_t &limit()noexcept { return *(uint16_t *)(data);}
	uint16_t limit()const noexcept { return *(uint16_t *)(data);}

	uint32_t &base()noexcept {return *(uint32_t *)(data + 2);}                // The address of the first element in our idt_entry_t array.
	uint32_t base()const noexcept {return *(uint32_t *)(data + 2);}
	uint8_t data[6];
};



struct idt_entry64_t
{
	uint16_t &base_lo()noexcept { return *(uint16_t *)(data);}
	uint16_t base_lo() const noexcept { return *(uint16_t *)(data);}

	uint16_t &sel()noexcept { return *(uint16_t *)(data+2); }
	uint16_t sel() const noexcept { return *(uint16_t *)(data+2); }

	uint8_t  &always0()noexcept { return *(data+4); }
	uint8_t  always0() const noexcept { return *(data+4); }

	uint8_t  &flags()noexcept { return *(data+5); }
	uint8_t  flags() const noexcept { return *(data+5); }

	uint16_t &base_mid()noexcept { return *(uint16_t *)(data+6); }
	uint16_t base_mid() const noexcept { return *(uint16_t *)(data+6); }
	
	uint32_t &base_hi()noexcept { return *(uint32_t *)(data+8); }
	uint32_t base_hi()const noexcept { return *(uint32_t *)(data+8); }
	
	uint32_t &reserved()noexcept { return *(uint32_t *)(data+12); }
	uint32_t reserved()const noexcept { return *(uint32_t *)(data+12); }

	void set(uint64_t base, uint16_t sel, uint8_t flags)
	{
		this->base_lo() = (uint16_t)(base & 0xFFFF);
		this->base_mid() = (uint16_t)((base >> 16) & 0xFFFF);

		this->sel()     = sel;
		this->always0() = 0;
		// We must uncomment the OR below when we get to using user-mode.
		// It sets the interrupt gate's privilege level to 3.
		this->flags()   = flags /* | 0x60 */;
		
		this->base_hi() = (uint32_t)( (base >> 32) & 0xFFFFFFFF );
		this->reserved() = 0;
	}

	uint8_t data[16];
};

struct idt_ptr64_t
{
	uint16_t &limit()noexcept { return *(uint16_t *)(data);}
	uint16_t limit()const noexcept { return *(uint16_t *)(data);}

	uint64_t &base()noexcept {return *(uint64_t *)(data + 2);}                // The address of the first element in our idt_entry_t array.
	uint64_t base()const noexcept {return *(uint64_t *)(data + 2);}
	uint8_t data[10];
};

#if 0

struct gdt_entry_struct
{
	uint16_t limit_low;           // The lower 16 bits of the limit.
	uint16_t base_low;            // The lower 16 bits of the base.
	uint8_t  base_middle;         // The next 8 bits of the base.
	uint8_t  access;              // Access flags, determine what ring this segment can be used in.
	uint8_t  granularity;
	uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
	uint16_t limit;               // The upper 16 bits of all selector limits.
	uint32_t base;                // The address of the first gdt_entry_t struct.
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;


// A struct describing an interrupt gate.
struct idt_entry_struct
{
	uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
	uint16_t sel;                 // Kernel segment selector.
	uint8_t  always0;             // This must always be zero.
	uint8_t  flags;               // More flags. See documentation.
	uint16_t base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct idt_ptr_struct
{
	uint16_t limit;
	uint32_t base;                // The address of the first element in our idt_entry_t array.
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
