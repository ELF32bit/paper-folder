#include "map.h"

#include <string.h>

#define MAP_MIN_CAPACITY 8

static inline
u8 _get_flag(const Array* flags, usize i) {
	if (flags->data == NULL) return MAP_FLAG_EMPTY;
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
void _map_array_destroy(Map* map, Array* array) {
	if (array->is_view) return;
	if (array->destroy != NULL) {
		FOR_EACH(i, array->capacity) {
			if (_get_flag(&map->flags, i) == MAP_FLAG_OCCUPIED) {
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

void map_create(Map* map, usize key_size, usize value_size,
	MapHashFunction hash, MapEqualsFunction equals)
{
	array_create(&map->keys, key_size);
	array_create(&map->values, value_size);
	array_create(&map->flags, sizeof(u8));
	map->size = 0;
	map->tombstones = 0;
	map->key_size = key_size;
	map->value_size = value_size;
	map->hash = hash;
	map->equals = equals;
	map->is_view = false;
}

void map_create_managed(Map* map, usize key_size, usize value_size,
	MapHashFunction hash, MapEqualsFunction equals,
	ArrayDestroyFunction key_destroy, ArrayCopyFunction key_copy,
	ArrayDestroyFunction value_destroy, ArrayCopyFunction value_copy)
{
	array_create_managed(&map->keys, key_size, key_destroy, key_copy);
	array_create_managed(&map->values, value_size, value_destroy, value_copy);
	array_create(&map->flags, sizeof(u8));
	map->size = 0;
	map->tombstones = 0;
	map->key_size = key_size;
	map->value_size = value_size;
	map->hash = hash;
	map->equals = equals;
	map->is_view = false;
}

void map_destroy(Map* map) {
	_map_array_destroy(map, &map->keys);
	_map_array_destroy(map, &map->values);
	array_destroy(&map->flags);
	map->size = 0;
	map->tombstones = 0;
	map->is_view = false;
}

void map_recreate(Map* map) {
	map_destroy(map);
}

void map_view(Map* map, const Map* source_map) {
	if (map == source_map) return;
	map_destroy(map);
	array_view(&map->keys, &source_map->keys);
	array_view(&map->values, &source_map->values);
	array_view(&map->flags, &source_map->flags);
	map->size = source_map->size;
	map->tombstones = source_map->tombstones;
	map->key_size = source_map->key_size;
	map->value_size = source_map->value_size;
	map->hash = source_map->hash;
	map->equals = source_map->equals;
	map->is_view = true;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline
usize _map_find_bucket(const Map* map, const void* key, bool* exists) {
	*exists = false;
	if (map->keys.capacity == 0) return 0;

	ASSERT((map->keys.capacity &
		(map->keys.capacity - 1)) == 0);

	usize first_tombstone = USIZE_MAX;
	usize mask = map->keys.capacity - 1;
	usize hash = map->hash(key);
	usize i = hash & mask;
	usize step = 1;

	while (true) {
		u8 flag = _get_flag(&map->flags, i);
		if (flag == MAP_FLAG_EMPTY) {
			return (first_tombstone != USIZE_MAX) ? first_tombstone : i;
		} else if (flag == MAP_FLAG_DELETED) {
			if (first_tombstone == USIZE_MAX) first_tombstone = i;
		} else if (flag == MAP_FLAG_OCCUPIED) {
			if (map->equals(key, array_get(&map->keys, i))) {
				*exists = true;
				return i;
			}
		}
		i = (i + step) & mask;
		step++;
	}
}

static inline
Error _map_reallocate(Map* map, usize new_capacity) {
	TRY_ADD(new_capacity, 3);
	Map new_map;
	map_create_managed(&new_map, map->key_size, map->value_size,
		map->hash, map->equals,
		map->keys.destroy, map->keys.copy,
		map->values.destroy, map->values.copy);

	usize old_capacity = map->keys.capacity;
	TRY(array_resize(&new_map.keys, new_capacity));
	TRY_OR_ELSE(array_resize(&new_map.values, new_capacity),
		map_destroy(&new_map));

	usize new_flags_size = (new_capacity + 3) >> 2;
	TRY_OR_ELSE(array_resize(&new_map.flags, new_flags_size),
		map_destroy(&new_map));

	memset(new_map.flags.data, MAP_FLAG_EMPTY,
		SIZEOF(u8, new_flags_size));

	if (old_capacity > 0) {
		FOR_EACH(i, old_capacity) {
			if (_get_flag(&map->flags, i) == MAP_FLAG_OCCUPIED) {
				void* old_key = array_get(&map->keys, i);
				void* old_value = array_get(&map->values, i);

				bool exists;
				usize new_bucket = _map_find_bucket(&new_map,
					old_key, &exists);

				array_set(&new_map.keys, new_bucket, old_key);
				array_set(&new_map.values, new_bucket, old_value);
				_set_flag(&new_map.flags, new_bucket, MAP_FLAG_OCCUPIED);
				new_map.size++;
			}
		}
	}

	free(map->keys.data);
	free(map->values.data);
	array_destroy(&map->flags);

	map->keys = new_map.keys;
	map->values = new_map.values;
	map->flags = new_map.flags;
	map->size = new_map.size;
	map->tombstones = 0;
	return OK;
}

Error map_reserve(Map* map, usize capacity) {
	ASSERT(NOT(map->is_view));
	if (capacity == 0) return OK;

	TRY_MULTIPLY(capacity, 3);
	usize required_capacity = (capacity * 3) / 2;
	if (map->keys.capacity >= required_capacity) return OK;

	usize new_capacity =
		usize_align_base2(required_capacity, MAP_MIN_CAPACITY);
	if (new_capacity == 0) return ERROR_INTEGER_OVERFLOW;

	return _map_reallocate(map, new_capacity);
}

Error map_add(Map* map, const void* key, const void* value, bool* exists) {
	ASSERT(NOT(map->is_view));
	if (exists != NULL) *exists = false;

	TRY_ADD(map->size, map->tombstones);
	TRY_ADD(map->size + map->tombstones, 1);
	TRY_MULTIPLY(map->size + map->tombstones + 1, 3);
	TRY_MULTIPLY(map->keys.capacity, 2);
	if ((map->size + map->tombstones + 1) * 3 >= map->keys.capacity * 2) {
		TRY(_map_reallocate(map, (map->keys.capacity != 0)
			? map->keys.capacity * 2
			: MAP_MIN_CAPACITY));
	}

	bool key_exists;
	usize bucket = _map_find_bucket(map, key, &key_exists);
	if (exists != NULL) *exists = key_exists;
	if (key_exists) {
		array_set(&map->values, bucket, value);
		return OK;
	}

	u8 old_flag = _get_flag(&map->flags, bucket);
	if (old_flag == MAP_FLAG_DELETED) map->tombstones--;

	array_set(&map->keys, bucket, key);
	array_set(&map->values, bucket, value);
	_set_flag(&map->flags, bucket, MAP_FLAG_OCCUPIED);
	map->size++;

	return OK;
}

bool map_remove(Map* map, const void* key) {
	ASSERT(NOT(map->is_view));
	bool exists;
	usize bucket = _map_find_bucket(map, key, &exists);
	if (exists) {
		if (map->keys.destroy != NULL) {
			map->keys.destroy(array_get(&map->keys, bucket));
		}
		if (map->values.destroy != NULL) {
			map->values.destroy(array_get(&map->values, bucket));
		}
		_set_flag(&map->flags, bucket, MAP_FLAG_DELETED);
		map->tombstones++;
		map->size--;
	}
	return exists;
}

bool map_has(const Map* map, const void* key) {
	bool exists;
	_map_find_bucket(map, key, &exists);
	return exists;
}

void* map_get(const Map* map, const void* key, const void* default_value) {
	bool exists;
	usize bucket = _map_find_bucket(map, key, &exists);
	if (exists) return array_get(&map->values, bucket);
	return (void*)default_value;
}

Error map_copy(Map* map, const Map* source_map) {
	ASSERT(map->key_size == source_map->key_size);
	ASSERT(map->value_size == source_map->value_size);
	ASSERT(map->equals == source_map->equals);
	ASSERT(map->keys.destroy == source_map->keys.destroy);
	ASSERT(map->keys.copy == source_map->keys.copy);
	ASSERT(map->values.destroy == source_map->values.destroy);
	ASSERT(map->values.copy == source_map->values.copy);
	ASSERT(NOT(map->is_view));

	map_recreate(map);
	TRY(map_reserve(map, source_map->size));

	u8 key[map->key_size];
	u8 value[map->value_size];
	MapIterator iterator = map_iterator(source_map);
	while (map_iterator_next(&iterator)) {
		bool key_copied = false;
		bool value_copied = false;

		if (map->keys.copy != NULL) {
			memset(key, 0, map->key_size);
			TRY(map->keys.copy(key, iterator.key));
			key_copied = true;
		}

		if (map->values.copy != NULL) {
			memset(value, 0, map->value_size);
			TRY_OR_ELSE(map->values.copy(value, iterator.value),
				if (key_copied && map->keys.destroy != NULL)
					map->keys.destroy(key));
			value_copied = true;
		}

		TRY_OR_ELSE(map_add(map,
			key_copied ? key : iterator.key,
			value_copied ? value : iterator.value, NULL),
			if (key_copied && map->keys.destroy != NULL)
				map->keys.destroy(key);
			if (value_copied && map->values.destroy != NULL)
				map->values.destroy(value));
	}

	return OK;
}

/* ========================================================================= */
/* Iterators & Traversal                                                     */
/* ========================================================================= */

MapIterator map_iterator(const Map* map) {
	MapIterator iterator;
	iterator.map = map;
	iterator.index = USIZE_MAX;
	iterator.key = NULL;
	iterator.value = NULL;
	return iterator;
}

bool map_iterator_next(MapIterator* iterator) {
	if (iterator->map->size == 0) return false;
	usize i = (iterator->index != USIZE_MAX)
		? iterator->index + 1 : 0;

	for (; i < iterator->map->keys.capacity; i++) {
		if (_get_flag(&iterator->map->flags, i) == MAP_FLAG_OCCUPIED) {
			iterator->index = i;
			iterator->key = array_get(&iterator->map->keys, i);
			iterator->value = array_get(&iterator->map->values, i);
			return true;
		}
	}

	iterator->index = iterator->map->keys.capacity;
	iterator->key = NULL;
	iterator->value = NULL;
	return false;
}