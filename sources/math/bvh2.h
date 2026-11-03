#ifndef COLLECTIONS_BVH2_H
#define COLLECTIONS_BVH2_H

#include "math/aabb2.h"
#include "collections/array.h"

#define BVH2_NONE USIZE_MAX

typedef bool (*BVH2BoundsQuery)(const void* context,
	usize object, usize type);

typedef bool (*BVH2TreeQuery)(const void* context,
	usize object1, usize type1, usize object2, usize type2);

typedef struct BVH2Node {
	AABB2 aabb;
	usize parent;
	usize children[4];
	usize object;
	usize type;
} BVH2Node;

typedef struct BVH2 {
	Array nodes, objects;
	usize first_free_node;
	usize root_node;
} BVH2;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void bvh2_create(BVH2* bvh);

void bvh2_destroy(BVH2* bvh);

void bvh2_recreate(BVH2* bvh);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

Error bvh2_insert(BVH2* bvh, usize object, AABB2 aabb, usize type);
void bvh2_remove(BVH2* bvh, usize object);

Error bvh2_update(BVH2* bvh, usize object, AABB2 aabb, usize type);
Error bvh2_update_aabb(BVH2* bvh, usize object, AABB2 aabb);
Error bvh2_update_type(BVH2* bvh, usize object, usize type);

Error bvh2_rebalance(BVH2* bvh);

/* ========================================================================= */
/* Queries                                                                   */
/* ========================================================================= */

void bvh2_query_aabb(const BVH2* bvh, AABB2 aabb,
	const void* context, BVH2BoundsQuery callback);

void bvh2_query_tree(const BVH2* bvh1, const BVH2* bvh2,
	const void* context, BVH2TreeQuery callback);

#endif /* COLLECTIONS_BVH2_H */