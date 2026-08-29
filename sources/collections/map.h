#ifndef COLLECTIONS_MAP_H
#define COLLECTIONS_MAP_H

#include "set.h"

#define MAP_FLAG_EMPTY 0
#define MAP_FLAG_DELETED 1
#define MAP_FLAG_OCCUPIED 2

typedef usize (*MapHashFunction)(const void*);
typedef bool (*MapEqualsFunction)(const void*, const void*);

typedef struct Map {
	Array keys, values, flags;
	usize size, tombstones;
	usize key_size, value_size;
	MapHashFunction hash;
	MapEqualsFunction equals;
	bool is_view;
} Map;

typedef struct MapIterator {
	const Map* map;
	usize index;
	void* key;
	void* value;
} MapIterator;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void map_create(Map* map, usize key_size, usize value_size,
	MapHashFunction hash, MapEqualsFunction equals);
void map_create_managed(Map* map, usize key_size, usize value_size,
	MapHashFunction hash, MapEqualsFunction equals,
	ArrayDestroyFunction key_destroy, ArrayCopyFunction key_copy,
	ArrayDestroyFunction value_destroy, ArrayCopyFunction value_copy);

#define MAP_CREATE(map, key_type, value_type) \
	Map map; \
	map_create(&map, sizeof(key_type), sizeof(value_type), \
		(MapHashFunction)key_type##_hash, \
		(MapEqualsFunction)key_type##_equals)

#define MAP_CREATE_MANAGED(map, key_type, value_type) \
	Map map; \
	map_create_managed(&map, sizeof(key_type), sizeof(value_type), \
		(MapHashFunction)key_type##_hash, \
		(MapEqualsFunction)key_type##_equals, \
		(ArrayDestroyFunction)key_type##_destroy, \
		(ArrayCopyFunction)key_type##_copy, \
		(ArrayDestroyFunction)value_type##_destroy, \
		(ArrayCopyFunction)value_type##_copy)

#define MAP_CREATE_MANAGED_KEYS(map, key_type, value_type) \
	Map map; \
	map_create_managed(&map, sizeof(key_type), sizeof(value_type), \
		(MapHashFunction)key_type##_hash, \
		(MapEqualsFunction)key_type##_equals, \
		(ArrayDestroyFunction)key_type##_destroy, \
		(ArrayCopyFunction)key_type##_copy, NULL, NULL) \

#define MAP_CREATE_MANAGED_VALUES(map, key_type, value_type) \
	Map map; \
	map_create_managed(&map, sizeof(key_type), sizeof(value_type), \
		(MapHashFunction)key_type##_hash, \
		(MapEqualsFunction)key_type##_equals, NULL, NULL, \
		(ArrayDestroyFunction)value_type##_destroy, \
		(ArrayCopyFunction)value_type##_copy)

#define Map_destroy map_destroy
void map_destroy(Map* map);

void map_recreate(Map* map);

void map_view(Map* map, const Map* source_map);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error map_reserve(Map* map, usize capacity);

Error map_add(Map* map, const void* key, const void* value, bool* exists);
bool map_remove(Map* map, const void* key);

bool map_has(const Map* map, const void* key);
void* map_get(const Map* map, const void* key, const void* default_value);

#define Map_copy map_copy
Error map_copy(Map* map, const Map* source_map);

/* ========================================================================= */
/* Iterators & Traversal                                                     */
/* ========================================================================= */

MapIterator map_iterator(const Map* map);
bool map_iterator_next(MapIterator* iterator);

#define MAP_ITERATE(map, iterator) \
	MapIterator iterator = map_iterator((map)); \
	while (map_iterator_next(&iterator))

#endif /* COLLECTIONS_MAP_H */