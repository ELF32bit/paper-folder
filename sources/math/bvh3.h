#ifndef COLLECTIONS_BVH3_H
#define COLLECTIONS_BVH3_H

#include "math/aabb3.h"
#include "collections/array.h"

#define BVH3_NONE USIZE_MAX

typedef bool (*BVH3BoundsQuery)(const void* context,
	usize object, usize type);

typedef bool (*BVH3TreeQuery)(const void* context,
	usize object1, usize type1, usize object2, usize type2);

typedef struct BVH3Node {
	AABB3 aabb;
	usize parent;
	usize children[8];
	usize object;
	usize type;
} BVH3Node;

typedef struct BVH3 {
	Array nodes, objects;
	usize first_free_node;
	usize root_node;
} BVH3;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void bvh3_create(BVH3* bvh);

void bvh3_destroy(BVH3* bvh);

void bvh3_recreate(BVH3* bvh);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error bvh3_insert(BVH3* bvh, usize object, AABB3 aabb, usize type);
void bvh3_remove(BVH3* bvh, usize object);

Error bvh3_update(BVH3* bvh, usize object, AABB3 aabb, usize type);
Error bvh3_update_aabb(BVH3* bvh, usize object, AABB3 aabb);
Error bvh3_update_type(BVH3* bvh, usize object, usize type);

Error bvh3_rebalance(BVH3* bvh);

/* ========================================================================= */
/* Queries                                                                   */
/* ========================================================================= */

void bvh3_query_aabb(const BVH3* bvh, AABB3 aabb,
	const void* context, BVH3BoundsQuery callback);

void bvh3_query_tree(const BVH3* bvh1, const BVH3* bvh2,
	const void* context, BVH3TreeQuery callback);

#endif /* COLLECTIONS_BVH3_H */