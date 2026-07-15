#ifndef COLLECTIONS_SET_H
#define COLLECTIONS_SET_H

#include "array.h"
#include "definitions/hash.h"
#include "definitions/equals.h"

#define SET_FLAG_EMPTY 0
#define SET_FLAG_DELETED 1
#define SET_FLAG_OCCUPIED 2

typedef usize (*SetHashFunction)(const void*);
typedef bool (*SetEqualsFunction)(const void*, const void*);

typedef struct Set {
	Array keys, flags;
	usize size, tombstones;
	usize key_size;
	SetHashFunction hash;
	SetEqualsFunction equals;
	bool is_view;
} Set;

typedef struct SetIterator {
	const Set* set;
	usize index;
	void* key;
} SetIterator;

void set_create(Set* set, usize key_size,
	SetHashFunction hash, SetEqualsFunction equals);
void set_create_managed(Set* set, usize key_size,
	SetHashFunction hash, SetEqualsFunction equals,
	ArrayDestroyFunction destroy, ArrayCopyFunction copy);

#define SET_CREATE(set, key_type) \
	Set set; \
	set_create(&set, sizeof(key_type), \
		(SetHashFunction)key_type##_hash, \
		(SetEqualsFunction)key_type##_equals)

#define SET_CREATE_MANAGED(set, key_type) \
	Set set; \
	set_create_managed(&set, sizeof(key_type), \
		(SetHashFunction)key_type##_hash, \
		(SetEqualsFunction)key_type##_equals, \
		(ArrayDestroyFunction)key_type##_destroy, \
		(ArrayCopyFunction)key_type##_copy)

#define Set_destroy set_destroy
void set_destroy(Set* set);
void set_recreate(Set* set);

void set_view(Set* set, const Set* source_set);

Error set_add(Set* set, const void* key, bool* exists);
bool set_remove(Set* set, const void* key);

bool set_has(const Set* set, const void* key);

#define Set_copy set_copy
Error set_copy(Set* set, const Set* source_set);

SetIterator set_iterator(const Set* set);
bool set_iterator_next(SetIterator* it);

#define SET_ITERATE(set, iterator) \
	SetIterator iterator = set_iterator((set)); \
	while (set_iterator_next(&iterator))

#endif /* COLLECTIONS_SET_H */