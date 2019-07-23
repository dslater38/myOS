#include <stdint.h>

extern "C"
{
	extern uint32_t end;
	uint64_t placement_address = (uint64_t)(&end);
}