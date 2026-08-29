#include "array2.h"

#include <stdlib.h>
#include <string.h>

static inline
void _array_destroy(Array* array, usize start, usize end) {
	if (array->destroy == NULL) return;
	FOR_EACH_IN_RANGE(i, start, end) {
		array->destroy(array_get(array, i));
	}
}

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void array2_create(Array2* array2, usize element_size) {
	array_create(&array2->data, element_size);
	array_create(&array2->offsets, sizeof(usize));
	array2->is_view = false;
}

void array2_create_managed(Array2* array2, usize element_size,
	ArrayDestroyFunction destroy, ArrayCopyFunction copy)
{
	array_create_managed(&array2->data, element_size, destroy, copy);
	array_create(&array2->offsets, sizeof(usize));
	array2->is_view = false;
}

void array2_destroy(Array2* array2) {
	array_destroy(&array2->data);
	array_destroy(&array2->offsets);
	array2->is_view = false;
}

void array2_recreate(Array2* array2) {
	array_recreate(&array2->data);
	array_recreate(&array2->offsets);
	array2->is_view = false;
}

void array2_view(Array2* array2, const Array2* source) {
	if (array2 == source) return;
	array2_destroy(array2);
	array_view(&array2->data, &source->data);
	array_view(&array2->offsets, &source->offsets);
	array2->is_view = true;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

usize array2_start_offset_at(const Array2* array2, usize index) {
	return (index == 0) ? 0
		: *(usize*)array_get(&array2->offsets, index - 1);
}

usize array2_end_offset_at(const Array2* array2, usize index) {
	return *(usize*)array_get(&array2->offsets, index);
}

usize array2_size_at(const Array2* array2, usize index) {
	return array2_end_offset_at(array2, index) -
		array2_start_offset_at(array2, index);
}

Error array2_copy(Array2* array2, const Array2* source) {
	ASSERT(array2->data.element_size == source->data.element_size);
	ASSERT(array2->data.destroy == source->data.destroy);
	ASSERT(array2->data.copy == source->data.copy);
	ASSERT(NOT(array2->is_view));

	Error result = array_copy(&array2->data, &source->data);
	if IS_ERROR(result) {
		usize new_offsets_size = 0;
		usize max_offset = array2->data.size;
		FOR_EACH(i, source->offsets.size) {
			usize* offset = array_get(&source->offsets, i);
			if (*offset > max_offset) break;
			new_offsets_size++;
		}

		Array new_offsets;
		array_create(&new_offsets, sizeof(usize));
		result = array_resize(&new_offsets, new_offsets_size);
		if IS_ERROR(result) {
			_array_destroy(&array2->data, 0, array2->data.size);
			array2->data.size = 0;
			array2->offsets.size = 0;
			return result;
		}

		if (new_offsets_size > 0) {
			FOR_EACH(i, new_offsets_size - 1) {
				usize* offset = array_get(&source->offsets, i);
				array_set(&new_offsets, i, offset);
			}
			array_set(&new_offsets,
				new_offsets_size - 1,
				&max_offset);
		}

		result = array_copy(&array2->offsets, &new_offsets);
		if IS_ERROR(result) {
			array_destroy(&new_offsets);
			_array_destroy(&array2->data, 0, array2->data.size);
			array2->data.size = 0;
			array2->offsets.size = 0;
			return result;
		}

		array_destroy(&new_offsets);
		return result;
	} else {
		result = array_copy(&array2->offsets, &source->offsets);
		if IS_ERROR(result) {
			usize new_data_size =
				array2_start_offset_at(array2, array2->offsets.size);
			_array_destroy(&array2->data, new_data_size, array2->data.size);
			array2->data.size = new_data_size;
			return result;
		}
	}

	return OK;
}

Error array2_append(Array2* array2, const Array* array) {
	ASSERT(array2->data.element_size == array->element_size);
	ASSERT(array2->data.destroy == array->destroy);
	ASSERT(array2->data.copy == array->copy);
	ASSERT(NOT(array2->is_view));

	usize old_size = array2->data.size;
	TRY(array_append_array(&array2->data, array));

	usize new_size = array2->data.size;
	TRY_OR_ELSE(array_append(&array2->offsets, &new_size),
		array2->data.size = old_size);

	return OK;
}

/* ========================================================================= */
/* Iterators & Traversal                                                     */
/* ========================================================================= */

Array2Iterator array2_iterator(const Array2* array2) {
	Array2Iterator iterator;
	iterator.array2 = array2;
	iterator.index = USIZE_MAX;
	iterator.start = 0;
	iterator.end = 0;
	iterator.size = 0;
	return iterator;
}

bool array2_iterator_next(Array2Iterator* iterator) {
	if (iterator->array2->offsets.size == 0) return false;
	usize i = (iterator->index != USIZE_MAX)
		? iterator->index + 1 : 0;

	if (i < iterator->array2->offsets.size) {
		iterator->index = i;
		iterator->start = array2_start_offset_at(iterator->array2, i);
		iterator->end = array2_end_offset_at(iterator->array2, i);
		iterator->size = iterator->end - iterator->start;
		return true;
	}

	iterator->index = iterator->array2->offsets.size;
	iterator->start = iterator->array2->data.size;
	iterator->end = iterator->array2->data.size;
	iterator->size = 0;
	return false;
}
