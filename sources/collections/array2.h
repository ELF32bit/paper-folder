#ifndef COLLECTIONS_ARRAY2_H
#define COLLECTIONS_ARRAY2_H

#include "array.h"

typedef struct Array2 {
	Array data;
	union {
		Array offsets;
		usize size;
	};
	bool is_view;
} Array2;

typedef struct Array2Iterator {
	const Array2* array2;
	usize index;
	usize start;
	usize end;
	usize size;
} Array2Iterator;

void array2_create(Array2* array2, usize element_size);
void array2_create_managed(Array2* array2, usize element_size,
	ArrayDestroyFunction destroy, ArrayCopyFunction copy);

#define ARRAY2_CREATE(array2, type) \
	Array2 array2; \
	array2_create(&array2, sizeof(type))

#define ARRAY2_CREATE_MANAGED(array2, type) \
	Array2 array2; \
	array2_create_managed(&array2, sizeof(type), \
		(ArrayDestroyFunction)type##_destroy, \
		(ArrayCopyFunction)type##_copy)

#define Array2_destroy array2_destroy
void array2_destroy(Array2* array2);
void array2_recreate(Array2* array2);

void array2_view(Array2* array2, const Array2* source);

usize array2_get_offset(const Array2* array2, usize index);
usize array2_get_size(const Array2* array2, usize index);

#define Array2_copy array2_copy
Error array2_copy(Array2* array2, const Array2* source);
Error array2_append(Array2* array2, const Array* array);

Array2Iterator array2_iterator(const Array2* array2);
bool array2_iterator_next(Array2Iterator* iterator);

#define ARRAY2_ITERATE(array2, iterator) \
	Array2Iterator iterator = array2_iterator((array2)); \
	while (array2_iterator_next(&iterator))

#define ARRAY2_FOR_EACH(array2, array, index, type, element) \
	ARRAY2_ITERATE((array2), array) \
		ARRAY_FOR_EACH_IN_RANGE(&(array2)->data, index, \
			type, element, array.start, array.end)

#endif /* COLLECTIONS_ARRAY2_H */
