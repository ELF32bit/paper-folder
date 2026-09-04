#ifndef COLLECTIONS_ARRAY_H
#define COLLECTIONS_ARRAY_H

#include "definitions/types.h"
#include "definitions/sort.h"

typedef void (*ArrayDestroyFunction)(void*);
typedef Error (*ArrayCopyFunction)(void*, const void*);
typedef int (*ArraySortFunction)(const void*, const void*);

typedef struct Array {
	usize size;
	void* data;
	usize capacity;
	usize element_size;
	ArrayDestroyFunction destroy;
	ArrayCopyFunction copy;
	bool is_view;
} Array;

typedef struct ArrayIterator {
	const Array* array;
	usize index;
	void* element;
} ArrayIterator;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void array_create(Array* array, usize element_size);
void array_create_managed(Array* array, usize element_size,
	ArrayDestroyFunction destroy, ArrayCopyFunction copy);

#define ARRAY_CREATE(array, type) \
	Array array; \
	array_create(&array, sizeof(type))

#define ARRAY_CREATE_MANAGED(array, type) \
	Array array; \
	array_create_managed(&array, sizeof(type), \
		(ArrayDestroyFunction)type##_destroy, \
		(ArrayCopyFunction)type##_copy)

#define Array_destroy array_destroy
void array_destroy(Array* array);

void array_recreate(Array* array);

void array_view(Array* array, const Array* source);

/* ========================================================================= */
/* Get & Set                                                                 */
/* ========================================================================= */

void* array_get(const Array* array, usize index);
void* array_get_start(const Array* array);
void* array_get_end(const Array* array);

void array_set(Array* array, usize index, const void* element);
void array_set_start(Array* array, const void* element);
void array_set_end(Array* array, const void* element);

#define ARRAY_SET(array, index, type, ...) do { \
	type _element = (type){__VA_ARGS__}; \
	array_set((array), (index), &_element); \
} while (0)

#define ARRAY_SET_START(array, index, type, ...) do { \
	type _element = (type){__VA_ARGS__}; \
	array_set_start((array), (index), &_element); \
} while (0)

#define ARRAY_SET_END(array, index, type, ...) do { \
	type _element = (type){__VA_ARGS__}; \
	array_set_end((array), (index), &_element); \
} while (0)

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error array_resize(Array* array, usize size);

#define Array_copy array_copy
Error array_copy(Array* array, const Array* source);

Error array_align_capacity(Array* array);
Error array_shrink_capacity(Array* array);

Error array_insert(Array* array, usize index, const void* element);

Error array_append(Array* array, const void* element);
Error array_append_array(Array* array, const Array* another);

#define ARRAY_APPEND(array, type, ...) do { \
	type _element = (type){__VA_ARGS__}; \
	array_append((array), &_element); \
} while (0)

void array_remove(Array* array, usize index);
void array_remove_range(Array* array, usize start, usize end);

void array_reverse_range(Array* array, usize start, usize end);
void array_reverse(Array* array);

/* ========================================================================= */
/* Sorting                                                                   */
/* ========================================================================= */

void array_sort(Array* array, ArraySortFunction compare);

void array_sort_range(Array* array, usize start, usize end,
	ArraySortFunction compare);

/* ========================================================================= */
/* Iterators & Traversal                                                     */
/* ========================================================================= */

ArrayIterator array_iterator(const Array* array);
bool array_iterator_next(ArrayIterator* iterator);

#define ARRAY_ITERATE(array, iterator) \
	ArrayIterator iterator = array_iterator((array)); \
	while (array_iterator_next(&iterator))

#define ARRAY_FOR_EACH(array, index, type, element) \
	for (usize index = 0, _state_##index = 0; \
		(index) < (array)->size && _state_##index != 2; \
		_state_##index == 1 \
			? ((index)++, _state_##index = 0) \
			: (_state_##index = 2)) \
		for (type element = array_get((array), (index)); \
			_state_##index == 0; \
			_state_##index = 1)

#define ARRAY_FOR_EACH_FROM_END(array, index, type, element) \
	for (usize index = (array)->size, _state_##index = 0; \
		(index) > 0 && _state_##index != 2; \
		_state_##index == 1 \
			? (_state_##index = 0) \
			: (_state_##index = 2)) \
		for (type element = array_get((array), --(index)); \
			_state_##index == 0; \
			_state_##index = 1)

#define ARRAY_FOR_EACH_IN_RANGE(array, index, type, element, start, end) \
	for (usize index = (start), _state_##index = 0; \
		(index) < (end) && _state_##index != 2; \
		_state_##index == 1 \
			? ((index)++, _state_##index = 0) \
			: (_state_##index = 2)) \
		for (type element = array_get((array), (index)); \
			_state_##index == 0; \
			_state_##index = 1)

#endif /* COLLECTIONS_ARRAY_H */