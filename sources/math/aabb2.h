#ifndef MATH_AABB2_H
#define MATH_AABB2_H

#include "vector2.h"

#define AABB2_EPSILON VECTOR2_EPSILON

typedef struct AABB2 {
	union {
		struct { Vector2 min, max; };
		real components[4];
		Vector2 points[2];
	};
} AABB2;

static inline AABB2 aabb2_empty(void) {
	return (AABB2){
		.min = {
			.x = REAL_INFINITY,
			.y = REAL_INFINITY
		},
		.max = {
			.x = -REAL_INFINITY,
			.y = -REAL_INFINITY
		}
	};
}

static inline bool aabb2_is_degenerate(AABB2 aabb) {
	return
		aabb.min.x > aabb.max.x ||
		aabb.min.y > aabb.max.y;
}

static inline bool aabb2_has_area(AABB2 aabb) {
	return
		aabb.max.x > aabb.min.x &&
		aabb.max.y > aabb.min.y;
}

static inline AABB2 aabb2_from_point(Vector2 point) {
	return (AABB2){ .min = point, .max = point };
}

static inline AABB2 aabb2_from_points(Vector2 point1, Vector2 point2) {
	return (AABB2){
		.min = vector2_min(point1, point2),
		.max = vector2_max(point1, point2)
	};
}

static inline Vector2 aabb2_clamp_point(AABB2 aabb, Vector2 point) {
	return vector2_clamp(point, aabb.min, aabb.max);
}

static inline AABB2 aabb2_abs(AABB2 aabb) {
	return (AABB2){
		.min = vector2_min(aabb.min, aabb.max),
		.max = vector2_max(aabb.min, aabb.max)
	};
}

static inline Vector2 aabb2_size(AABB2 aabb) {
	return vector2_subtract(aabb.max, aabb.min);
}

static inline Vector2 aabb2_center(AABB2 aabb) {
	return vector2_scale(vector2_add(aabb.min, aabb.max), 0.5);
}

static inline real aabb2_area(AABB2 aabb) {
	Vector2 size = aabb2_size(aabb);
	return size.x * size.y;
}

static inline real aabb2_perimeter(AABB2 aabb) {
	Vector2 size = aabb2_size(aabb);
	return (size.x + size.y) * 2.0;
}

static inline bool aabb2_has_point(AABB2 aabb, Vector2 point) {
	return
		point.x >= aabb.min.x &&
		point.x <= aabb.max.x &&
		point.y >= aabb.min.y &&
		point.y <= aabb.max.y;
}

static inline bool aabb2_intersects(AABB2 aabb, AABB2 other) {
	return
		aabb.min.x <= other.max.x &&
		aabb.max.x >= other.min.x &&
		aabb.min.y <= other.max.y &&
		aabb.max.y >= other.min.y;
}

static inline AABB2 aabb2_intersect(AABB2 aabb, AABB2 other) {
	return (AABB2){
		.min = vector2_max(aabb.min, other.min),
		.max = vector2_min(aabb.max, other.max)
	};
}

static inline bool aabb2_encloses(AABB2 aabb, AABB2 other) {
	return
		other.min.x >= aabb.min.x &&
		other.max.x <= aabb.max.x &&
		other.min.y >= aabb.min.y &&
		other.max.y <= aabb.max.y;
}

static inline AABB2 aabb2_merge(AABB2 aabb, AABB2 other) {
	return (AABB2){
		.min = vector2_min(aabb.min, other.min),
		.max = vector2_max(aabb.max, other.max)
	};
}

static inline AABB2 aabb2_expand(AABB2 aabb, Vector2 point) {
	return (AABB2){
		.min = vector2_min(aabb.min, point),
		.max = vector2_max(aabb.max, point)
	};
}

static inline AABB2 aabb2_grow(AABB2 aabb, real amount) {
	Vector2 offset = { .x = amount,.y = amount };
	return (AABB2){
		.min = vector2_subtract(aabb.min, offset),
		.max = vector2_add(aabb.max, offset)
	};
}

static inline int aabb2_shortest_axis_index(AABB2 aabb) {
	return vector2_min_axis_index(aabb2_size(aabb));
}

static inline int aabb2_longest_axis_index(AABB2 aabb) {
	return vector2_max_axis_index(aabb2_size(aabb));
}

static inline real aabb2_axis_center(AABB2 aabb, int axis) {
	return (axis == 0)
		? (aabb.min.x + aabb.max.x) * 0.5
		: (aabb.min.y + aabb.max.y) * 0.5;
}

static inline real aabb2_axis_size(AABB2 aabb, int axis) {
	return (axis == 0)
		? aabb.max.x - aabb.min.x
		: aabb.max.y - aabb.min.y;
}

static inline bool aabb2_is_equal(AABB2 aabb, AABB2 other, real epsilon) {
	return
		vector2_is_equal(aabb.min, other.min, epsilon) &&
		vector2_is_equal(aabb.max, other.max, epsilon);
}

#define AABB2_equals aabb2_equals
static inline bool aabb2_equals(AABB2 aabb, AABB2 other) {
	return aabb2_is_equal(aabb, other, AABB2_EPSILON);
}

static inline bool aabb2_is_point(AABB2 aabb, real epsilon) {
	return vector2_is_zero(aabb2_size(aabb), epsilon);
}

#endif /* MATH_AABB2_H */