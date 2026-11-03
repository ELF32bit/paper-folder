#ifndef DEFINITIONS_EXTRA_H
#define DEFINITIONS_EXTRA_H

#include "integer.h"
#include "cpu.h"

#define SWAP(a, b) do { \
	__typeof__(a) _tmp = (a); \
	(a) = (b); (b) = _tmp; \
} while (0)

#define CLAMP(value, min, max) (MIN(MAX((value), (min)), (max)))

#define SIZEOF(type, count) (sizeof(type) * USIZE((count)))
#define BUFFER_SIZE(buffer) (sizeof((buffer)) / sizeof((buffer)[0]))

static inline usize usize_align_base2(usize value, usize min) {
	value = MAX(value, min);
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
#if defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	value |= value >> 8;
#endif
#if defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	value |= value >> 16;
#endif
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	value |= value >> 32;
#endif
	return value + 1;
}

#endif /* DEFINITIONS_EXTRA_H */