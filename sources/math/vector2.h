#ifndef MATH_VECTOR2_H
#define MATH_VECTOR2_H

#include "definitions/types.h"
#include "definitions/math.h"
#include "definitions/hash.h"

#define VECTOR2_EPSILON \
	SWITCH_BASED_ON_REAL_TYPE \
		(1e-5F, 1e-11, 1e-14L)

typedef struct Vector2 {
	union {
		struct { real x, y; };
		real components[2];
	};
} Vector2;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Vector2 vector2_zero(void) {
	Vector2 v;
	v.x = 0.0;
	v.y = 0.0;
	return v;
}

static inline Vector2 vector2_from_components(real x, real y) {
	Vector2 v;
	v.x = x;
	v.y = y;
	return v;
}

/* ========================================================================= */
/* Operators                                                                 */
/* ========================================================================= */

static inline Vector2 vector2_add(Vector2 a, Vector2 b) {
	return vector2_from_components(
		a.x + b.x, a.y + b.y);
}

static inline Vector2 vector2_subtract(Vector2 a, Vector2 b) {
	return vector2_from_components(
		a.x - b.x, a.y - b.y);
}

static inline Vector2 vector2_multiply(Vector2 a, Vector2 b) {
	return vector2_from_components(
		a.x * b.x, a.y * b.y);
}

static inline Vector2 vector2_divide(Vector2 a, Vector2 b) {
	return vector2_from_components(
		a.x / b.x, a.y / b.y);
}

static inline Vector2 vector2_scale(Vector2 v, real s) {
	return vector2_from_components(
		v.x * s, v.y * s);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline real vector2_dot(Vector2 a, Vector2 b) {
	return (a.x * b.x) + (a.y * b.y);
}

static inline real vector2_cross(Vector2 a, Vector2 b) {
	return (a.x * b.y) - (a.y * b.x);
}

static inline Vector2 vector2_lerp(Vector2 a, Vector2 b, real t) {
	return vector2_from_components(
		a.x * (1.0 - t) + b.x * t,
		a.y * (1.0 - t) + b.y * t);
}

static inline real vector2_length2(Vector2 v) {
	return vector2_dot(v, v);
}

static inline real vector2_length(Vector2 v) {
	return SQRT(vector2_length2(v));
}

static inline Vector2 vector2_normalize(Vector2 v) {
	real l = vector2_length(v);
	return (l == 0.0)
		? vector2_zero()
		: vector2_from_components(
			v.x / l, v.y / l);
}

static inline Vector2 vector2_direction(Vector2 a, Vector2 b) {
	return vector2_subtract(b, a);
}

static inline Vector2 vector2_direction_normalized(Vector2 a, Vector2 b) {
	return vector2_normalize(vector2_direction(a, b));
}

static inline real vector2_distance2(Vector2 a, Vector2 b) {
	return vector2_length2(vector2_subtract(b, a));
}

static inline real vector2_distance(Vector2 a, Vector2 b) {
	return vector2_length(vector2_subtract(b, a));
}

static inline Vector2 vector2_abs(Vector2 v) {
	return vector2_from_components(
		ABS(v.x), ABS(v.y));
}

static inline Vector2 vector2_clamp(Vector2 v, Vector2 min, Vector2 max) {
	return vector2_from_components(
		CLAMP(v.x, min.x, max.x),
		CLAMP(v.y, min.y, max.y));
}

static inline Vector2 vector2_snap(Vector2 v, Vector2 step) {
	return vector2_from_components(
		(step.x != 0.0) ? ROUND(v.x / step.x) * step.x : v.x,
		(step.y != 0.0) ? ROUND(v.y / step.y) * step.y : v.y);
}

static inline Vector2 vector2_bounce(Vector2 v, Vector2 normal) {
	return vector2_subtract(v, vector2_scale(normal,
		2.0 * vector2_dot(v, normal)));
}

static inline Vector2 vector2_reflect(Vector2 v, Vector2 normal) {
	return vector2_add(v, vector2_scale(normal,
		2.0 * vector2_dot(v, normal)));
}

static inline Vector2 vector2_slide(Vector2 v, Vector2 normal) {
	return vector2_subtract(v, vector2_scale(normal,
		vector2_dot(v, normal)));
}

static inline Vector2 vector2_limit_length(Vector2 v, real max_length) {
	real l = vector2_length(v);
	return (l > max_length && l > 0.0)
		? vector2_scale(v, max_length / l)
		: v;
}

static inline Vector2 vector2_rotate(Vector2 v, real angle) {
	real c = COS(angle); real s = SIN(angle);
	return vector2_from_components(
		v.x * c - v.y * s,
		v.x * s + v.y * c);
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline Vector2 vector2_min(Vector2 a, Vector2 b) {
	return vector2_from_components(
		MIN(a.x, b.x), MIN(a.y, b.y));
}

static inline real vector2_min_axis(Vector2 v) {
	return (v.x < v.y) ? v.x : v.y;
}

static inline int vector2_min_axis_index(Vector2 v) {
	return (v.x < v.y) ? 0 : 1;
}

static inline Vector2 vector2_max(Vector2 a, Vector2 b) {
	return vector2_from_components(
		MAX(a.x, b.x), MAX(a.y, b.y));
}

static inline real vector2_max_axis(Vector2 v) {
	return (v.x < v.y) ? v.y : v.x;
}

static inline int vector2_max_axis_index(Vector2 v) {
	return (v.x < v.y) ? 1 : 0;
}

static inline bool vector2_is_normalized(Vector2 v) {
	return ABS(vector2_length2(v) - 1.0) < VECTOR2_EPSILON;
}

static inline Vector2 vector2_orthogonal(Vector2 v) {
	return vector2_from_components(
		-v.y, v.x);
}

static inline real vector2_angle(Vector2 v) {
	return ATAN2(v.y, v.x);
}

static inline real vector2_angle_to(Vector2 a, Vector2 b) {
	return ATAN2(vector2_cross(a, b), vector2_dot(a, b));
}

static inline Vector2 vector2_sign(Vector2 v) {
	return vector2_from_components(
		(v.x > 0.0) ? 1.0 : ((v.x < 0.0) ? -1.0 : 0.0),
		(v.y > 0.0) ? 1.0 : ((v.y < 0.0) ? -1.0 : 0.0));
}

static inline bool vector2_is_equal(Vector2 a, Vector2 b, real epsilon) {
	return
		ABS(a.x - b.x) < epsilon &&
		ABS(a.y - b.y) < epsilon;
}

#define Vector2_equals vector2_equals
static inline bool vector2_equals(Vector2 a, Vector2 b) {
	return vector2_is_equal(a, b, VECTOR2_EPSILON);
}

static inline bool vector2_is_zero(Vector2 a, real epsilon) {
	return
		ABS(a.x) < epsilon &&
		ABS(a.y) < epsilon;
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Vector2_hash vector2_hash
static inline usize vector2_hash(Vector2 v) {
	return hash_real_mix2(v.x, v.y);
}

#endif /* MATH_VECTOR2_H */