#ifndef IDT64_H_INCLUDED
#define IDT64_H_INCLUDED
#include "common.h"

struct idt_ptr64_t
{
	u16int &limit()noexcept { return *(u16int *)(data);}
	u16int limit()const noexcept { return *(u16int *)(data);}

	u64int &base()noexcept {return *(u64int *)(data + 2);}                // The address of the first element in our idt_entry_t array.
	u64int base()const noexcept {return *(u64int *)(data + 2);}
	u8int data[10];
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

    u32int &base_hi()noexcept { return *(u32int *)(data+8);}
    u32int base_hi()const noexcept { return *(u32int *)(data+8);}

    u32int &reserved()noexcept { return *(u32int *)(data+12);}
    u32int reserved()const noexcept { return *(u32int *)(data+12);}

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
	}

	u8int data[16];
};

#endif // IDT64_H_INCLUDED
