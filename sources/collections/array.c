#include "array.h"

#include <stdlib.h>
#include <string.h>

#include "definitions/cpu.h"

#define ARRAY_MIN_CAPACITY 4

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void array_create(Array* array, usize element_size) {
	ASSERT(element_size != 0);
	array->data = NULL;
	array->capacity = 0;
	array->size = 0;
	array->element_size = element_size;
	array->destroy = NULL;
	array->copy = NULL;
	array->is_view = false;
}

void array_create_managed(Array* array, usize element_size,
	ArrayDestroyFunction destroy, ArrayCopyFunction copy) {
	array_create(array, element_size);
	array->destroy = destroy;
	array->copy = copy;
}

static inline
void _array_destroy(Array* array, usize start, usize end) {
	if (array->destroy == NULL) return;
	FOR_EACH_IN_RANGE(i, start, end) {
		array->destroy(array_get(array, i));
	}
}

void array_destroy(Array* array) {
	if NOT(array->is_view) {
		_array_destroy(array, 0, array->size);
		free(array->data);
	}
	array->data = NULL;
	array->capacity = 0;
	array->size = 0;
	array->is_view = false;
}

void array_recreate(Array* array) {
	array_destroy(array);
}

void array_view(Array* array, const Array* source) {
	if (array == source) return;
	array_destroy(array);
	array->data = source->data;
	array->capacity = source->capacity;
	array->size = source->size;
	array->element_size = source->element_size;
	array->destroy = source->destroy;
	array->copy = source->copy;
	array->is_view = true;
}

/* ========================================================================= */
/* Get & Set                                                                 */
/* ========================================================================= */

static inline
void* _array_get(const Array* array, usize index) {
	return (u8*)array->data + index * array->element_size;
}

void* array_get(const Array* array, usize index) {
	ASSERT(index < array->size);
	return _array_get(array, index);
}

void* array_get_start(const Array* array) {
	return array_get(array, 0);
}

void* array_get_end(const Array* array) {
	return array_get(array, array->size - 1);
}

void array_set(Array* array, usize index, const void* element) {
	ASSERT(NOT(array->is_view));
	ASSERT(index < array->size);
	ASSERT(element != NULL);

	void* old_element = array_get(array, index);
	if (old_element == element) return;

	if (array->destroy == NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			(u8*)old_element, (u8*)old_element + array->element_size,
			(u8*)element, (u8*)element + array->element_size,
			ASSERT_ERROR(false, "element overlaps old array element"));
	} else {
		TRY_MEMORY_REGIONS_OR_ERROR(
			(u8*)_array_get(array, 0),
			(u8*)_array_get(array, array->size),
			(u8*)element, (u8*)element + array->element_size,
			ASSERT_ERROR(false, "element overlaps managed array memory"));
		array->destroy(old_element);
	}

	memcpy(old_element, element, array->element_size);
}

void array_set_start(Array* array, const void* element) {
	array_set(array, 0, element);
}

void array_set_end(Array* array, const void* element) {
	array_set(array, array->size - 1, element);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error array_resize(Array* array, usize size) {
	ASSERT(NOT(array->is_view));

	usize old_size = array->size;
	if (size < old_size) {
		_array_destroy(array, size, old_size);
		array->size = size;
	}

	TRY_MULTIPLY(size, array->element_size);
	void* new_data = realloc(array->data, size * array->element_size);
	if (new_data == NULL && size > 0) return ERROR_OUT_OF_MEMORY;

	if (size > old_size) {
		memset((u8*)new_data + old_size * array->element_size, 0,
			(size - old_size) * array->element_size);
	}

	array->data = new_data;
	array->capacity = size;
	array->size = size;
	return OK;
}

Error array_copy(Array* array, const Array* source) {
	ASSERT(array->element_size == source->element_size);
	ASSERT(array->destroy == source->destroy);
	ASSERT(array->copy == source->copy);
	ASSERT(NOT(array->is_view));

	if (array == source) return OK;
	if (array->data != NULL && source->data != NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			(u8*)_array_get(array, 0),
			(u8*)_array_get(array, array->capacity),
			(u8*)_array_get(source, 0),
			(u8*)_array_get(source, source->size),
			ASSERT_ERROR(false, "array memory overlaps source"));
	}

	array_recreate(array);
	if (source->size == 0 ||
		source->data == NULL) {
		return OK;
	}

	TRY(array_resize(array, source->size));

	if (array->copy != NULL) {
		FOR_EACH(i, source->size) {
			TRY_OR_ELSE(source->copy(
				_array_get(array, i),
				array_get(source, i)),
				array->size = i);
		}
	} else {
		memcpy(array->data, source->data,
			source->size * source->element_size);
	}

	array->size = source->size;
	return OK;
}

Error array_align_capacity(Array* array) {
	ASSERT(NOT(array->is_view));

	usize new_capacity =
		usize_align_base2(array->capacity, ARRAY_MIN_CAPACITY);
	if (new_capacity == 0) return ERROR_INTEGER_OVERFLOW;

	TRY_MULTIPLY(new_capacity, array->element_size);
	void* new_data = realloc(array->data,
		new_capacity * array->element_size);
	TRY_NEW_MEMORY(new_data);

	array->data = new_data;
	array->capacity = new_capacity;
	return OK;
}

Error array_shrink_capacity(Array* array) {
	TRY(array_resize(array, array->size));
	return OK;
}

Error array_insert(Array* array, usize index, const void* element) {
	ASSERT(NOT(array->is_view));
	ASSERT(index <= array->size);
	ASSERT(element != NULL);

	u8 buffer[array->element_size];
	if (array->destroy == NULL) {
		memcpy(buffer, element, array->element_size);
		element = buffer;
	} else if (array->data != NULL) {
		TRY_MEMORY_REGIONS_OR_ERROR(
			(u8*)_array_get(array, 0),
			(u8*)_array_get(array, array->capacity),
			(u8*)element, (u8*)element + array->element_size,
			ASSERT_ERROR(false, "element overlaps managed array memory"));
	}

	if (array->size == array->capacity) {
		usize new_capacity;
		if (array->capacity == 0) {
			new_capacity = ARRAY_MIN_CAPACITY;
		} else {
			TRY_MULTIPLY(array->capacity, 2);
			new_capacity = array->capacity * 2;
		}

		TRY_MULTIPLY(new_capacity, array->element_size);
		void* new_data = realloc(array->data,
			new_capacity * array->element_size);
		TRY_NEW_MEMORY(new_data);

		array->data = new_data;
		array->capacity = new_capacity;
	}

	usize move_size = array->size - index;
	void* old_element = _array_get(array, index);
	if (move_size > 0) {
		void* next_element = _array_get(array, index + 1);
		memmove(next_element, old_element,
			move_size * array->element_size);
	}

	memcpy(old_element, element, array->element_size);

	array->size++;
	return OK;
}

Error array_append(Array* array, const void* element) {
	return array_insert(array, array->size, element);
}

Error array_append_array(Array* array, const Array* another) {
	ASSERT(array->element_size == another->element_size);
	ASSERT(array->destroy == another->destroy);
	ASSERT(array->copy == another->copy);
	ASSERT(NOT(array->is_view));

	if (another->size == 0 ||
		another->data == NULL) return OK;

	TRY_ADD(array->size, another->size);
	usize new_size = array->size + another->size;

	if (array != another) {
		if (array->data != NULL) {
			TRY_MEMORY_REGIONS_OR_ERROR(
				(u8*)_array_get(array, 0),
				(u8*)_array_get(array, array->capacity),
				(u8*)_array_get(another, 0),
				(u8*)_array_get(another, another->size),
				ASSERT_ERROR(false, "array memory overlaps another"));
		}

		if (new_size > array->capacity) {
			usize old_capacity = array->capacity;
			array->capacity = new_size;
			TRY_OR_ELSE(array_align_capacity(array),
				array->capacity = old_capacity);
		}

		memcpy(_array_get(array, array->size), another->data,
			another->size * another->element_size);

		array->size = new_size;
	} else {
		void* new_data = malloc(new_size * array->element_size);
		TRY_NEW_MEMORY(new_data);

		memcpy(new_data, array->data, array->size * array->element_size);
		memcpy((u8*)new_data + array->size * array->element_size,
			another->data, another->size * another->element_size);

		free(array->data);
		array->data = new_data;
		array->capacity = new_size;
		array->size = new_size;
	}

	return OK;
}

void array_remove(Array* array, usize index) {
	ASSERT(NOT(array->is_view));

	void* element = array_get(array, index);
	if (array->destroy != NULL) {
		array->destroy(element);
	}

	usize move_size = array->size - index - 1;
	if (move_size > 0) {
		void* next_element = array_get(array, index + 1);
		memmove(element, next_element,
			move_size * array->element_size);
	}

	array->size--;
}

void array_remove_range(Array* array, usize start, usize end) {
	ASSERT(NOT(array->is_view));
	ASSERT(end <= array->size);
	ASSERT(start <= end);

	if (start == end) return;
	_array_destroy(array, start, end);

	usize move_size = array->size - end;
	if (move_size > 0) {
		void* start_element = array_get(array, start);
		void* end_element = _array_get(array, end);
		memmove(start_element, end_element,
			move_size * array->element_size);
	}

	array->size -= (end - start);
}

void array_reverse(Array* array) {
	ASSERT(NOT(array->is_view));
	if (array->size <= 1) return;

	u8 buffer[array->element_size];
	u8* left_element = array_get_start(array);
	u8* right_element = array_get_end(array);

	while (left_element < right_element) {
		memcpy(buffer, left_element, array->element_size);
		memcpy(left_element, right_element, array->element_size);
		memcpy(right_element, buffer, array->element_size);
		right_element -= array->element_size;
		left_element += array->element_size;
	}
}

/* ========================================================================= */
/* Sorting                                                                   */
/* ========================================================================= */

void array_sort(Array* array,
	ArraySortFunction compare) {
	ASSERT(NOT(array->is_view));
	if (array->size <= 1) return;
	qsort(array->data, array->size,
		array->element_size, compare);
}

void array_sort_range(Array* array, usize start, usize end,
	ArraySortFunction compare) {
	ASSERT(NOT(array->is_view));
	ASSERT(end <= array->size);
	ASSERT(start <= end);
	if (end - start <= 1) return;
	qsort(array_get(array, start), end - start,
		array->element_size, compare);
}

/* ========================================================================= */
/* Iterators & Traversal                                                     */
/* ========================================================================= */

ArrayIterator array_iterator(const Array* array) {
	ArrayIterator iterator;
	iterator.array = array;
	iterator.index = USIZE_MAX;
	iterator.element = NULL;
	return iterator;
}

bool array_iterator_next(ArrayIterator* iterator) {
	if (iterator->array->size == 0) return false;
	usize i = (iterator->index != USIZE_MAX)
		? iterator->index + 1 : 0;

	if (i < iterator->array->size) {
		iterator->index = i;
		iterator->element = array_get(iterator->array, i);
		return true;
	}

	iterator->index = iterator->array->size;
	iterator->element = NULL;
	return false;
}