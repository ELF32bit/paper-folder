#ifndef MATH_AABB3_H
#define MATH_AABB3_H

#include "vector3.h"

#define AABB3_EPSILON VECTOR3_EPSILON

typedef struct AABB3 {
	union {
		struct { Vector3 min, max; };
		real components[6];
		Vector3 points[2];
	};
} AABB3;

static inline AABB3 aabb3_empty(void) {
	return (AABB3){
		.min = {
			.x = REAL_INFINITY,
			.y = REAL_INFINITY,
			.z = REAL_INFINITY
		},
		.max = {
			.x = -REAL_INFINITY,
			.y = -REAL_INFINITY,
			.z = -REAL_INFINITY
		}
	};
}

static inline bool aabb3_is_degenerate(AABB3 aabb) {
	return
		aabb.min.x > aabb.max.x ||
		aabb.min.y > aabb.max.y ||
		aabb.min.z > aabb.max.z;
}

static inline bool aabb3_has_volume(AABB3 aabb) {
	return
		aabb.max.x > aabb.min.x &&
		aabb.max.y > aabb.min.y &&
		aabb.max.z > aabb.min.z;
}

static inline AABB3 aabb3_from_point(Vector3 point) {
	return (AABB3){ .min = point, .max = point };
}

static inline AABB3 aabb3_from_points(Vector3 point1, Vector3 point2) {
	return (AABB3){
		.min = vector3_min(point1, point2),
		.max = vector3_max(point1, point2)
	};
}

static inline Vector3 aabb3_clamp_point(AABB3 aabb, Vector3 point) {
	return vector3_clamp(point, aabb.min, aabb.max);
}

static inline AABB3 aabb3_abs(AABB3 aabb) {
	return (AABB3){
		.min = vector3_min(aabb.min, aabb.max),
		.max = vector3_max(aabb.min, aabb.max)
	};
}

static inline Vector3 aabb3_size(AABB3 aabb) {
	return vector3_subtract(aabb.max, aabb.min);
}

static inline Vector3 aabb3_center(AABB3 aabb) {
	return vector3_scale(vector3_add(aabb.min, aabb.max), 0.5);
}

static inline real aabb3_volume(AABB3 aabb) {
	Vector3 size = aabb3_size(aabb);
	return size.x * size.y * size.z;
}

static inline real aabb3_surface_area(AABB3 aabb) {
	Vector3 size = aabb3_size(aabb);
	return (size.x * size.y + size.y * size.z + size.z * size.x) * 2.0;
}

static inline bool aabb3_has_point(AABB3 aabb, Vector3 point) {
	return
		point.x >= aabb.min.x &&
		point.x <= aabb.max.x &&
		point.y >= aabb.min.y &&
		point.y <= aabb.max.y &&
		point.z >= aabb.min.z &&
		point.z <= aabb.max.z;
}

static inline bool aabb3_intersects(AABB3 aabb, AABB3 other) {
	return
		aabb.min.x <= other.max.x &&
		aabb.max.x >= other.min.x &&
		aabb.min.y <= other.max.y &&
		aabb.max.y >= other.min.y &&
		aabb.min.z <= other.max.z &&
		aabb.max.z >= other.min.z;
}

static inline AABB3 aabb3_intersect(AABB3 aabb, AABB3 other) {
	return (AABB3){
		.min = vector3_max(aabb.min, other.min),
		.max = vector3_min(aabb.max, other.max)
	};
}

static inline bool aabb3_encloses(AABB3 aabb, AABB3 other) {
	return
		other.min.x >= aabb.min.x &&
		other.max.x <= aabb.max.x &&
		other.min.y >= aabb.min.y &&
		other.max.y <= aabb.max.y &&
		other.min.z >= aabb.min.z &&
		other.max.z <= aabb.max.z;
}

static inline AABB3 aabb3_merge(AABB3 aabb, AABB3 other) {
	return (AABB3){
		.min = vector3_min(aabb.min, other.min),
		.max = vector3_max(aabb.max, other.max)
	};
}

static inline AABB3 aabb3_expand(AABB3 aabb, Vector3 point) {
	return (AABB3){
		.min = vector3_min(aabb.min, point),
		.max = vector3_max(aabb.max, point)
	};
}

static inline AABB3 aabb3_grow(AABB3 aabb, real amount) {
	Vector3 offset = { .x = amount, .y = amount, .z = amount };
	return (AABB3){
		.min = vector3_subtract(aabb.min, offset),
		.max = vector3_add(aabb.max, offset)
	};
}

static inline int aabb3_shortest_axis_index(AABB3 aabb) {
	return vector3_min_axis_index(aabb3_size(aabb));
}

static inline int aabb3_longest_axis_index(AABB3 aabb) {
	return vector3_max_axis_index(aabb3_size(aabb));
}

static inline real aabb3_axis_center(AABB3 aabb, int axis) {
	return (axis == 0)
		? (aabb.min.x + aabb.max.x) * 0.5 : (axis == 1)
		? (aabb.min.y + aabb.max.y) * 0.5
		: (aabb.min.z + aabb.max.z) * 0.5;
}

static inline real aabb3_axis_size(AABB3 aabb, int axis) {
	return (axis == 0)
		? aabb.max.x - aabb.min.x : (axis == 1)
		? aabb.max.y - aabb.min.y
		: aabb.max.z - aabb.min.z;
}

static inline bool aabb3_is_equal(AABB3 aabb, AABB3 other, real epsilon) {
	return
		vector3_is_equal(aabb.min, other.min, epsilon) &&
		vector3_is_equal(aabb.max, other.max, epsilon);
}

#define AABB3_equals aabb3_equals
static inline bool aabb3_equals(AABB3 aabb, AABB3 other) {
	return aabb3_is_equal(aabb, other, AABB3_EPSILON);
}

static inline bool aabb3_is_point(AABB3 aabb, real epsilon) {
	return vector3_is_zero(aabb3_size(aabb), epsilon);
}

#endif /* MATH_AABB3_H */