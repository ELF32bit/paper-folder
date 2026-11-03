#include "bvh2.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void bvh2_create(BVH2* bvh) {
	array_create(&bvh->nodes, sizeof(BVH2Node));
	array_create(&bvh->objects, sizeof(usize));
	bvh->first_free_node = BVH2_NONE;
	bvh->root_node = BVH2_NONE;
}

void bvh2_destroy(BVH2* bvh) {
	array_destroy(&bvh->nodes);
	array_destroy(&bvh->objects);
	bvh->first_free_node = BVH2_NONE;
	bvh->root_node = BVH2_NONE;
}

void bvh2_recreate(BVH2* bvh) {
	array_recreate(&bvh->nodes);
	array_recreate(&bvh->objects);
	bvh->first_free_node = BVH2_NONE;
	bvh->root_node = BVH2_NONE;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

/* ========================================================================= */
/* Queries                                                                   */
/* ========================================================================= */
