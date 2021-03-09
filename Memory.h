#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

#ifdef BYTE_ORDER_BIG_ENDIAN
#define BigEndian32
#else
#define BigEndian32 Swap32
#endif

size_t CompressionBufferSize(const size_t original_size);

uint32_t Swap32(const uint32_t value);

#endif
