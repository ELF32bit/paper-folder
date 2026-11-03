#ifndef DEFINITIONS_FOR_H
#define DEFINITIONS_FOR_H

#include "assert.h"
#include "integer.h"

#define FOR_EACH(index, size) \
	for (usize index = 0; (index) < USIZE((size)); (index)++)

#define FOR_EACH_FROM_END(index, size) \
	for (usize index = USIZE((size)); (index)-- > 0;)

#define FOR_EACH_IN_RANGE(index, start, end) \
	for (usize index = USIZE((start)); (index) < USIZE((end)); (index)++)

#define FOR_EACH_IN_RANGE_FROM_END(index, start, end) \
	for (usize index = USIZE((end)); (index)-- > USIZE((start));)

static inline
usize wrap_index(usize index, isize offset, usize size) {
	if (size == 0) return 0;
	usize i = index % size;
	return (offset >= 0)
		? ((i + (USIZE(offset) % size)) % size)
		: ((i + size - (USIZE(-offset) % size)) % size);
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
		? (start + ((i + (USIZE(offset) % size)) % size))
		: (start + ((i + size - (USIZE(-offset) % size)) % size));
}

#endif /* DEFINITIONS_FOR_H */