#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include "definitions/types.h"
#include "definitions/math.h"
#include "definitions/hash.h"

#define VECTOR3_EPSILON \
	SWITCH_BASED_ON_REAL_TYPE \
		(1e-5F, 1e-11, 1e-14L)

typedef struct Vector3 {
	union {
		struct { real x, y, z; };
		real components[3];
	};
} Vector3;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Vector3 vector3_zero(void) {
	Vector3 v;
	v.x = 0.0;
	v.y = 0.0;
	v.z = 0.0;
	return v;
}

static inline Vector3 vector3_from_components(real x, real y, real z) {
	Vector3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

/* ========================================================================= */
/* Operators                                                                 */
/* ========================================================================= */

static inline Vector3 vector3_add(Vector3 a, Vector3 b) {
	return vector3_from_components(
		a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vector3 vector3_subtract(Vector3 a, Vector3 b) {
	return vector3_from_components(
		a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vector3 vector3_multiply(Vector3 a, Vector3 b) {
	return vector3_from_components(
		a.x * b.x, a.y * b.y, a.z * b.z);
}

static inline Vector3 vector3_divide(Vector3 a, Vector3 b) {
	return vector3_from_components(
		a.x / b.x, a.y / b.y, a.z / b.z);
}

static inline Vector3 vector3_scale(Vector3 v, real s) {
	return vector3_from_components(
		v.x * s, v.y * s, v.z * s);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline real vector3_dot(Vector3 a, Vector3 b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline Vector3 vector3_cross(Vector3 a, Vector3 b) {
	return vector3_from_components(
		(a.y * b.z) - (a.z * b.y),
		(a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x));
}

static inline Vector3 vector3_lerp(Vector3 a, Vector3 b, real t) {
	return vector3_from_components(
		a.x * (1.0 - t) + b.x * t,
		a.y * (1.0 - t) + b.y * t,
		a.z * (1.0 - t) + b.z * t);
}

static inline real vector3_length2(Vector3 v) {
	return vector3_dot(v, v);
}

static inline real vector3_length(Vector3 v) {
	return SQRT(vector3_length2(v));
}

static inline Vector3 vector3_normalize(Vector3 v) {
	real l = vector3_length(v);
	return (l == 0.0)
		? vector3_zero()
		: vector3_from_components(
			v.x / l, v.y / l, v.z / l);
}

static inline Vector3 vector3_direction(Vector3 a, Vector3 b) {
	return vector3_subtract(b, a);
}

static inline Vector3 vector3_direction_normalized(Vector3 a, Vector3 b) {
	return vector3_normalize(vector3_direction(a, b));
}

static inline real vector3_distance2(Vector3 a, Vector3 b) {
	return vector3_length2(vector3_subtract(b, a));
}

static inline real vector3_distance(Vector3 a, Vector3 b) {
	return vector3_length(vector3_subtract(b, a));
}

static inline Vector3 vector3_abs(Vector3 v) {
	return vector3_from_components(ABS(v.x), ABS(v.y), ABS(v.z));
}

static inline Vector3 vector3_clamp(Vector3 v, Vector3 min, Vector3 max) {
	return vector3_from_components(
		CLAMP(v.x, min.x, max.x),
		CLAMP(v.y, min.y, max.y),
		CLAMP(v.z, min.z, max.z));
}

static inline Vector3 vector3_snap(Vector3 v, Vector3 step) {
	return vector3_from_components(
		(step.x != 0.0) ? ROUND(v.x / step.x) * step.x : v.x,
		(step.y != 0.0) ? ROUND(v.y / step.y) * step.y : v.y,
		(step.z != 0.0) ? ROUND(v.z / step.z) * step.z : v.z);
}

static inline Vector3 vector3_bounce(Vector3 v, Vector3 normal) {
	return vector3_subtract(v, vector3_scale(normal,
		2.0 * vector3_dot(v, normal)));
}

static inline Vector3 vector3_reflect(Vector3 v, Vector3 normal) {
	return vector3_add(v, vector3_scale(normal,
		2.0 * vector3_dot(v, normal)));
}

static inline Vector3 vector3_slide(Vector3 v, Vector3 normal) {
	return vector3_subtract(v, vector3_scale(normal,
		vector3_dot(v, normal)));
}

static inline Vector3 vector3_limit_length(Vector3 v, real max_length) {
	real l = vector3_length(v);
	return (l > max_length && l > 0.0)
		? vector3_scale(v, max_length / l)
		: v;
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline Vector3 vector3_min(Vector3 a, Vector3 b) {
	return vector3_from_components(
		MIN(a.x, b.x), MIN(a.y, b.y), MIN(a.z, b.z));
}

static inline real vector3_min_axis(Vector3 v) {
	real min_xy = (v.x < v.y) ? v.x : v.y;
	return (min_xy < v.z) ? min_xy : v.z;
}

static inline int vector3_min_axis_index(Vector3 v) {
	return (v.x < v.y)
		? (v.x < v.z) ? 0 : 2
		: (v.y < v.z) ? 1 : 2;
}

static inline Vector3 vector3_max(Vector3 a, Vector3 b) {
	return vector3_from_components(
		MAX(a.x, b.x), MAX(a.y, b.y), MAX(a.z, b.z));
}

static inline real vector3_max_axis(Vector3 v) {
	real max_xy = (v.x < v.y) ? v.y : v.x;
	return (max_xy < v.z) ? v.z : max_xy;
}

static inline int vector3_max_axis_index(Vector3 v) {
	return (v.x < v.y)
		? (v.y < v.z) ? 2 : 1
		: (v.x < v.z) ? 2 : 0;
}

static inline bool vector3_is_normalized(Vector3 v) {
	return ABS(vector3_length2(v) - 1.0) < VECTOR3_EPSILON;
}

static inline real vector3_angle_to(Vector3 a, Vector3 b) {
	if (vector3_length2(a) * vector3_length2(b) == 0.0) return 0.0;
	return ATAN2(vector3_length(vector3_cross(a, b)), vector3_dot(a, b));
}

static inline Vector3 vector3_sign(Vector3 v) {
	return vector3_from_components(
		(v.x > 0.0) ? 1.0 : ((v.x < 0.0) ? -1.0 : 0.0),
		(v.y > 0.0) ? 1.0 : ((v.y < 0.0) ? -1.0 : 0.0),
		(v.z > 0.0) ? 1.0 : ((v.z < 0.0) ? -1.0 : 0.0));
}

static inline bool vector3_is_equal(Vector3 a, Vector3 b, real epsilon) {
	return
		ABS(a.x - b.x) < epsilon &&
		ABS(a.y - b.y) < epsilon &&
		ABS(a.z - b.z) < epsilon;
}

#define Vector3_equals vector3_equals
static inline bool vector3_equals(Vector3 a, Vector3 b) {
	return vector3_is_equal(a, b, VECTOR3_EPSILON);
}

static inline bool vector3_is_zero(Vector3 a, real epsilon) {
	return
		ABS(a.x) < epsilon &&
		ABS(a.y) < epsilon &&
		ABS(a.z) < epsilon;
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Vector3_hash vector3_hash
static inline usize vector3_hash(Vector3 v) {
	return hash_real_mix3(v.x, v.y, v.z);
}

#endif /* MATH_VECTOR3_H */