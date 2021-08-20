#include <stdint.h>

extern "C"
{
	extern void (*kernel_end)();
	uint64_t placement_address = reinterpret_cast<uint64_t >(&kernel_end);
}