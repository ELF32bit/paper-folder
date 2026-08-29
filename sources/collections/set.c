#include "set.h"

#include <string.h>

#define SET_MIN_CAPACITY 8

static inline
u8 _get_flag(const Array* flags, usize i) {
	if (flags->data == NULL) return SET_FLAG_EMPTY;
	ASSERT((i >> 2) < flags->capacity);
	u8* byte = array_get(flags, i >> 2);
	return (*byte >> ((i & 3) << 1)) & 3;
}

static inline
void _set_flag(Array* flags, usize i, u8 value) {
	ASSERT((i >> 2) < flags->capacity && value <= 3);
	usize shift = (i & 3) << 1;
	u8* byte = array_get(flags, i >> 2);
	*byte = (*byte & ~(3 << shift)) | (value << shift);
}

static inline
void _set_array_destroy(Set* set, Array* array) {
	if (array->is_view) return;
	if (array->destroy != NULL) {
		FOR_EACH(i, array->capacity) {
			if (_get_flag(&set->flags, i) == SET_FLAG_OCCUPIED) {
				array->destroy(array_get(array, i));
			}
		}
	}
	free(array->data);
	array->data = NULL;
	array->capacity = 0;
	array->size = 0;
	array->is_view = false;
}

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void set_create(Set* set, usize key_size,
	SetHashFunction hash, SetEqualsFunction equals)
{
	array_create(&set->keys, key_size);
	array_create(&set->flags, sizeof(u8));
	set->size = 0;
	set->tombstones = 0;
	set->key_size = key_size;
	set->hash = hash;
	set->equals = equals;
	set->is_view = false;
}

void set_create_managed(Set* set, usize key_size,
	SetHashFunction hash, SetEqualsFunction equals,
	ArrayDestroyFunction destroy, ArrayCopyFunction copy)
{
	array_create_managed(&set->keys, key_size, destroy, copy);
	array_create(&set->flags, sizeof(u8));
	set->size = 0;
	set->tombstones = 0;
	set->key_size = key_size;
	set->hash = hash;
	set->equals = equals;
	set->is_view = false;
}

void set_destroy(Set* set) {
	_set_array_destroy(set, &set->keys);
	array_destroy(&set->flags);
	set->size = 0;
	set->tombstones = 0;
	set->is_view = false;
}

void set_recreate(Set* set) {
	set_destroy(set);
}

void set_view(Set* set, const Set* source_set) {
	if (set == source_set) return;
	set_destroy(set);
	array_view(&set->keys, &source_set->keys);
	array_view(&set->flags, &source_set->flags);
	set->size = source_set->size;
	set->tombstones = source_set->tombstones;
	set->key_size = source_set->key_size;
	set->hash = source_set->hash;
	set->equals = source_set->equals;
	set->is_view = true;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline
usize _set_find_bucket(const Set* set, const void* key, bool* exists) {
	*exists = false;
	if (set->keys.capacity == 0) return 0;

	ASSERT((set->keys.capacity &
		(set->keys.capacity - 1)) == 0);

	usize first_tombstone = USIZE_MAX;
	usize mask = set->keys.capacity - 1;
	usize hash = set->hash(key);
	usize i = hash & mask;
	usize step = 1;

	while (true) {
		u8 flag = _get_flag(&set->flags, i);
		if (flag == SET_FLAG_EMPTY) {
			return (first_tombstone != USIZE_MAX) ? first_tombstone : i;
		} else if (flag == SET_FLAG_DELETED) {
			if (first_tombstone == USIZE_MAX) first_tombstone = i;
		} else if (flag == SET_FLAG_OCCUPIED) {
			if (set->equals(key, array_get(&set->keys, i))) {
				*exists = true;
				return i;
			}
		}
		i = (i + step) & mask;
		step++;
	}
}

static inline
Error _set_reallocate(Set* set, usize new_capacity) {
	TRY_ADD(new_capacity, 3);
	Set new_set;
	set_create_managed(&new_set, set->key_size,
		set->hash, set->equals,
		set->keys.destroy, set->keys.copy);

	usize old_capacity = set->keys.capacity;
	TRY(array_resize(&new_set.keys, new_capacity));

	usize new_flags_size = (new_capacity + 3) >> 2;
	TRY_OR_ELSE(array_resize(&new_set.flags, new_flags_size),
		set_destroy(&new_set));

	memset(new_set.flags.data, SET_FLAG_EMPTY,
		SIZEOF(u8, new_flags_size));

	if (old_capacity > 0) {
		FOR_EACH(i, old_capacity) {
			if (_get_flag(&set->flags, i) == SET_FLAG_OCCUPIED) {
				void* old_key = array_get(&set->keys, i);

				bool exists;
				usize new_bucket = _set_find_bucket(&new_set,
					old_key, &exists);

				array_set(&new_set.keys, new_bucket, old_key);
				_set_flag(&new_set.flags, new_bucket, SET_FLAG_OCCUPIED);
				new_set.size++;
			}
		}
	}

	free(set->keys.data);
	array_destroy(&set->flags);
	set->keys = new_set.keys;
	set->flags = new_set.flags;
	set->size = new_set.size;
	set->tombstones = 0;
	return OK;
}

Error set_reserve(Set* set, usize capacity) {
	ASSERT(NOT(set->is_view));
	if (capacity == 0) return OK;

	TRY_MULTIPLY(capacity, 3);
	usize required_capacity = (capacity * 3) / 2;
	if (set->keys.capacity >= required_capacity) return OK;

	usize new_capacity =
		usize_align_base2(required_capacity, SET_MIN_CAPACITY);
	if (new_capacity == 0) return ERROR_INTEGER_OVERFLOW;

	return _set_reallocate(set, new_capacity);
}

Error set_add(Set* set, const void* key, bool* exists) {
	ASSERT(NOT(set->is_view));
	if (exists != NULL) *exists = false;

	TRY_ADD(set->size, set->tombstones);
	TRY_ADD(set->size + set->tombstones, 1);
	TRY_MULTIPLY(set->size + set->tombstones + 1, 3);
	TRY_MULTIPLY(set->keys.capacity, 2);
	if ((set->size + set->tombstones + 1) * 3 >= set->keys.capacity * 2) {
		TRY(_set_reallocate(set, (set->keys.capacity != 0)
			? set->keys.capacity * 2
			: SET_MIN_CAPACITY));
	}

	bool key_exists;
	usize bucket = _set_find_bucket(set, key, &key_exists);
	if (exists != NULL) *exists = key_exists;
	if (key_exists) return OK;

	u8 old_flag = _get_flag(&set->flags, bucket);
	if (old_flag == SET_FLAG_DELETED) set->tombstones--;

	array_set(&set->keys, bucket, key);
	_set_flag(&set->flags, bucket, SET_FLAG_OCCUPIED);
	set->size++;

	return OK;
}

bool set_remove(Set* set, const void* key) {
	ASSERT(NOT(set->is_view));
	bool exists;
	usize bucket = _set_find_bucket(set, key, &exists);
	if (exists) {
		if (set->keys.destroy != NULL) {
			set->keys.destroy(array_get(&set->keys, bucket));
		}
		_set_flag(&set->flags, bucket, SET_FLAG_DELETED);
		set->tombstones++;
		set->size--;
	}
	return exists;
}

bool set_has(const Set* set, const void* key) {
	bool exists;
	_set_find_bucket(set, key, &exists);
	return exists;
}

Error set_copy(Set* set, const Set* source_set) {
	ASSERT(set->key_size == source_set->key_size);
	ASSERT(set->equals == source_set->equals);
	ASSERT(set->keys.destroy == source_set->keys.destroy);
	ASSERT(set->keys.copy == source_set->keys.copy);
	ASSERT(NOT(set->is_view));

	set_recreate(set);
	TRY(set_reserve(set, source_set->size));

	u8 key[set->key_size];
	SetIterator iterator = set_iterator(source_set);
	while (set_iterator_next(&iterator)) {
		if (set->keys.copy != NULL) {
			memset(key, 0, set->key_size);
			TRY(set->keys.copy(key, iterator.key));
			TRY_OR_ELSE(set_add(set, key, NULL),
				if (set->keys.destroy != NULL)
					set->keys.destroy(key));
		} else {
			TRY(set_add(set, iterator.key, NULL));
		}
	}

	return OK;
}

/* ========================================================================= */
/* Iterators & Traversal                                                     */
/* ========================================================================= */

SetIterator set_iterator(const Set* set) {
	SetIterator iterator;
	iterator.set = set;
	iterator.index = USIZE_MAX;
	iterator.key = NULL;
	return iterator;
}

bool set_iterator_next(SetIterator* iterator) {
	if (iterator->set->size == 0) return false;
	usize i = (iterator->index != USIZE_MAX)
		? iterator->index + 1 : 0;

	for (; i < iterator->set->keys.capacity; i++) {
		if (_get_flag(&iterator->set->flags, i) == SET_FLAG_OCCUPIED) {
			iterator->index = i;
			iterator->key = array_get(&iterator->set->keys, i);
			return true;
		}
	}

	iterator->index = iterator->set->keys.capacity;
	iterator->key = NULL;
	return false;
}