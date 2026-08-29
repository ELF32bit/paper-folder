#ifndef DEFINITIONS_EXTRA_H
#define DEFINITIONS_EXTRA_H

#include "assert.h"
#include "integer.h"
#include "cpu.h"

#define SWAP(a, b) do { \
	__typeof__(a) _tmp = (a); \
	(a) = (b); (b) = _tmp; \
} while (0)

#define CLAMP(value, min, max) (MIN(MAX((value), (min)), (max)))

#define SIZEOF(type, count) (sizeof(type) * (usize)(count))
#define BUFFER_SIZE(buffer) ((sizeof((buffer))) / (sizeof((buffer)[0])))

#define FOR_EACH(index, count) \
	for (usize index = 0; (index) < (usize)(count); (index)++)

#define FOR_EACH_FROM_END(index, count) \
	for (usize index = (usize)(count); (index)-- > 0;)

#define FOR_EACH_IN_RANGE(index, start, end) \
	for (usize index = (usize)(start); (index) < (usize)(end); (index)++)

static inline
usize wrap_index(usize index, isize offset, usize size) {
	if (size == 0) return 0;
	usize i = index % size;
	return (offset >= 0)
		? ((i + ((usize)offset % size)) % size)
		: ((i + size - ((usize)-offset % size)) % size);
}

static inline
usize wrap_index_in_range(usize index, isize offset, usize start, usize end) {
	ASSERT(start <= end);
	usize size = end - start;
	if (size == 0) return start;
	usize i = (index >= start)
		? ((index - start) % size)
		: ((size - ((start - index) % size)) % size);
	return (offset >= 0)
		? (start + ((i + ((usize)offset % size)) % size))
		: (start + ((i + size - ((usize)-offset % size)) % size));
}

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