#ifndef MATH_VECTOR2_H
#define MATH_VECTOR2_H

#include "definitions/types.h"
#include "definitions/math.h"

#define VECTOR2_EPSILON _Generic((real)0, \
	float: 1e-5F, \
	long double: 1e-14L, \
	default: 1e-11 \
)

typedef struct Vector2 {
	union {
		struct { real x, y; };
		real components[2];
	};
} Vector2;

static inline Vector2 vector2_zero(void) {
	return (Vector2){ .x = 0.0, .y = 0.0 };
}

static inline Vector2 vector2_add(Vector2 a, Vector2 b) {
	return (Vector2){
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}

static inline Vector2 vector2_subtract(Vector2 a, Vector2 b) {
	return (Vector2){
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}

static inline Vector2 vector2_multiply(Vector2 a, Vector2 b) {
	return (Vector2){
		.x = a.x * b.x,
		.y = a.y * b.y
	};
}

static inline Vector2 vector2_divide(Vector2 a, Vector2 b) {
	return (Vector2){
		.x = a.x / b.x,
		.y = a.y / b.y
	};
}

static inline Vector2 vector2_scale(Vector2 v, real s) {
	return (Vector2){
		.x = v.x * s,
		.y = v.y * s
	};
}

static inline real vector2_dot(Vector2 a, Vector2 b) {
	return (a.x * b.x) + (a.y * b.y);
}

static inline real vector2_cross(Vector2 a, Vector2 b) {
	return (a.x * b.y) - (a.y * b.x);
}

static inline Vector2 vector2_lerp(Vector2 a, Vector2 b, real t) {
	return (Vector2){
		.x = a.x * (1.0 - t) + b.x * t,
		.y = a.y * (1.0 - t) + b.y * t
	};
}

static inline real vector2_length_squared(Vector2 v) {
	return vector2_dot(v, v);
}

static inline real vector2_length(Vector2 v) {
	return SQRT(vector2_length_squared(v));
}

static inline Vector2 vector2_normalize(Vector2 v) {
	real l = vector2_length(v);
	if (l == 0.0) {
		return vector2_zero();
	} else {
		return (Vector2){
			.x = v.x / l,
			.y = v.y / l
		};
	}
}

static inline Vector2 vector2_direction(Vector2 a, Vector2 b, bool normalize) {
	return normalize
		? vector2_normalize(vector2_subtract(b, a))
		: vector2_subtract(b, a);
}

static inline real vector2_distance_squared(Vector2 a, Vector2 b) {
	return vector2_length_squared(vector2_subtract(b, a));
}

static inline real vector2_distance(Vector2 a, Vector2 b) {
	return vector2_length(vector2_subtract(b, a));
}

static inline Vector2 vector2_abs(Vector2 v) {
	return (Vector2){
		.x = ABS(v.x),
		.y = ABS(v.y)
	};
}

static inline Vector2 vector2_min(Vector2 a, Vector2 b) {
	return (Vector2){
		.x = MIN(a.x, b.x),
		.y = MIN(a.y, b.y)
	};
}

static inline real vector2_min_axis(Vector2 v) {
	return v.x < v.y ? v.x : v.y;
}

static inline int vector2_min_axis_index(Vector2 v) {
	return v.x < v.y ? 0 : 1;
}

static inline Vector2 vector2_max(Vector2 a, Vector2 b) {
	return (Vector2){
		.x = MAX(a.x, b.x),
		.y = MAX(a.y, b.y)
	};
}

static inline real vector2_max_axis(Vector2 v) {
	return v.x < v.y ? v.y : v.x;
}

static inline int vector2_max_axis_index(Vector2 v) {
	return v.x < v.y ? 1 : 0;
}

static inline bool vector2_is_normalized(Vector2 v) {
	return ABS(vector2_length_squared(v) - 1.0) < VECTOR2_EPSILON;
}

static inline Vector2 vector2_orthogonal(Vector2 v) {
	return (Vector2){
		.x = -v.y,
		.y = v.x
	};
}

static inline real vector2_angle(Vector2 v) {
	return ATAN2(v.y, v.x);
}

static inline real vector2_angle_to(Vector2 a, Vector2 b) {
	return ATAN2(vector2_cross(a, b), vector2_dot(a, b));
}

static inline Vector2 vector2_rotate(Vector2 v, real angle) {
	real c = COS(angle);
	real s = SIN(angle);
	return (Vector2){
		.x = v.x * c - v.y * s,
		.y = v.x * s + v.y * c
	};
}

static inline Vector2 vector2_bounce(Vector2 v, Vector2 normal) {
	return vector2_subtract(v,
		vector2_scale(normal, 2.0 * vector2_dot(v, normal)));
}

static inline Vector2 vector2_reflect(Vector2 v, Vector2 normal) {
	return vector2_add(v,
		vector2_scale(normal, 2.0 * vector2_dot(v, normal)));
}

static inline Vector2 vector2_slide(Vector2 v, Vector2 normal) {
	return vector2_subtract(v,
		vector2_scale(normal, vector2_dot(v, normal)));
}

static inline Vector2 vector2_limit_length(Vector2 v, real max_length) {
	real l = vector2_length(v);
	if (l > max_length && l > 0.0) {
		return vector2_scale(v, max_length / l);
	} else return v;
}

static inline Vector2 vector2_clamp(Vector2 v, Vector2 min, Vector2 max) {
	return (Vector2){
		.x = CLAMP(v.x, min.x, max.x),
		.y = CLAMP(v.y, min.y, max.y)
	};
}

static inline Vector2 vector2_sign(Vector2 v) {
	return (Vector2){
		.x = v.x > 0.0 ? 1.0 : ((v.x < 0.0) ? -1.0 : 0.0),
		.y = v.y > 0.0 ? 1.0 : ((v.y < 0.0) ? -1.0 : 0.0)
	};
}

static inline Vector2 vector2_snap(Vector2 v, Vector2 step) {
	return (Vector2){
		.x = (step.x != 0.0) ? ROUND(v.x / step.x) * step.x : v.x,
		.y = (step.y != 0.0) ? ROUND(v.y / step.y) * step.y : v.y
	};
}

static inline bool vector2_is_equal(Vector2 a, Vector2 b, real epsilon) {
	return
		ABS(a.x - b.x) < epsilon &&
		ABS(a.y - b.y) < epsilon;
}

static inline bool vector2_is_zero(Vector2 a, real epsilon) {
	return
		ABS(a.x) < epsilon &&
		ABS(a.y) < epsilon;
}

#endif /* MATH_VECTOR2_H */