#include "bvh3.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void bvh3_create(BVH3* bvh) {
	array_create(&bvh->nodes, sizeof(BVH3Node));
	array_create(&bvh->objects, sizeof(usize));
	bvh->first_free_node = BVH3_NONE;
	bvh->root_node = BVH3_NONE;
}

void bvh3_destroy(BVH3* bvh) {
	array_destroy(&bvh->nodes);
	array_destroy(&bvh->objects);
	bvh->first_free_node = BVH3_NONE;
	bvh->root_node = BVH3_NONE;
}

void bvh3_recreate(BVH3* bvh) {
	array_recreate(&bvh->nodes);
	array_recreate(&bvh->objects);
	bvh->first_free_node = BVH3_NONE;
	bvh->root_node = BVH3_NONE;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

/* ========================================================================= */
/* Queries                                                                   */
/* ========================================================================= */
