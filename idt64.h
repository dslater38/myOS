#ifndef IDT64_H_INCLUDED
#define IDT64_H_INCLUDED
#include "common.h"

struct idt_ptr64_t
{
	uint16_t &limit()noexcept { return *(uint16_t *)(data);}
	uint16_t limit()const noexcept { return *(uint16_t *)(data);}

	uint64_t &base()noexcept {return *(uint64_t *)(data + 2);}                // The address of the first element in our idt_entry_t array.
	uint64_t base()const noexcept {return *(uint64_t *)(data + 2);}
	uint8_t data[10];
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

    uint32_t &base_hi()noexcept { return *(uint32_t *)(data+8);}
    uint32_t base_hi()const noexcept { return *(uint32_t *)(data+8);}

    uint32_t &reserved()noexcept { return *(uint32_t *)(data+12);}
    uint32_t reserved()const noexcept { return *(uint32_t *)(data+12);}

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
	}

	uint8_t data[16];
};

#endif // IDT64_H_INCLUDED
