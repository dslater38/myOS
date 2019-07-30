#include <stdint.h>

extern "C"
{
	extern void (*end)();
	uint64_t placement_address = reinterpret_cast<uint64_t >(&end);
}