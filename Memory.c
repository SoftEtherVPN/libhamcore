#include "Memory.h"

size_t CompressionBufferSize(const size_t original_size) { return original_size * 2 + 256; }

uint32_t Swap32(const uint32_t value)
{
	uint32_t swapped;
	((uint8_t *)&swapped)[0] = ((uint8_t *)&value)[3];
	((uint8_t *)&swapped)[1] = ((uint8_t *)&value)[2];
	((uint8_t *)&swapped)[2] = ((uint8_t *)&value)[1];
	((uint8_t *)&swapped)[3] = ((uint8_t *)&value)[0];
	return swapped;
}
