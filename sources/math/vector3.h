#ifndef MATH_VECTOR3_H
#define MATH_VECTOR3_H

#include "definitions/types.h"
#include "definitions/math.h"

#define VECTOR3_EPSILON _Generic((real)0, \
	float: 1e-5F, \
	long double: 1e-14L, \
	default: 1e-11 \
)

typedef struct Vector3 {
	union {
		struct { real x, y, z; };
		real components[3];
	};
} Vector3;

static inline Vector3 vector3_zero(void) {
	return (Vector3){ .x = 0.0, .y = 0.0, .z = 0.0 };
}

static inline Vector3 vector3_add(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
}

static inline Vector3 vector3_subtract(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
}

static inline Vector3 vector3_multiply(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z
	};
}

static inline Vector3 vector3_divide(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = a.x / b.x,
		.y = a.y / b.y,
		.z = a.z / b.z
	};
}

static inline Vector3 vector3_scale(Vector3 v, real s) {
	return (Vector3){
		.x = v.x * s,
		.y = v.y * s,
		.z = v.z * s
	};
}

static inline real vector3_dot(Vector3 a, Vector3 b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline Vector3 vector3_cross(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = (a.y * b.z) - (a.z * b.y),
		.y = (a.z * b.x) - (a.x * b.z),
		.z = (a.x * b.y) - (a.y * b.x)
	};
}

static inline Vector3 vector3_lerp(Vector3 a, Vector3 b, real t) {
	return (Vector3){
		.x = a.x * (1.0 - t) + b.x * t,
		.y = a.y * (1.0 - t) + b.y * t,
		.z = a.z * (1.0 - t) + b.z * t
	};
}

static inline real vector3_length2(Vector3 v) {
	return vector3_dot(v, v);
}

static inline real vector3_length(Vector3 v) {
	return SQRT(vector3_length2(v));
}

static inline Vector3 vector3_normalize(Vector3 v) {
	real l = vector3_length(v);
	if (l == 0.0) {
		return vector3_zero();
	} else {
		return (Vector3){
			.x = v.x / l,
			.y = v.y / l,
			.z = v.z / l
		};
	}
}

static inline Vector3 vector3_direction(Vector3 a, Vector3 b, bool normalize) {
	return normalize
		? vector3_normalize(vector3_subtract(b, a))
		: vector3_subtract(b, a);
}

static inline real vector3_distance2(Vector3 a, Vector3 b) {
	return vector3_length2(vector3_subtract(b, a));
}

static inline real vector3_distance(Vector3 a, Vector3 b) {
	return vector3_length(vector3_subtract(b, a));
}

static inline Vector3 vector3_abs(Vector3 v) {
	return (Vector3){
		.x = ABS(v.x),
		.y = ABS(v.y),
		.z = ABS(v.z)
	};
}

static inline Vector3 vector3_min(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = MIN(a.x, b.x),
		.y = MIN(a.y, b.y),
		.z = MIN(a.z, b.z)
	};
}

static inline real vector3_min_axis(Vector3 v) {
	real min_xy = v.x < v.y ? v.x : v.y;
	return min_xy < v.z ? min_xy : v.z;
}

static inline int vector3_min_axis_index(Vector3 v) {
	if (v.x < v.y) {
		return v.x < v.z ? 0 : 2;
	} else {
		return v.y < v.z ? 1 : 2;
	}
}

static inline Vector3 vector3_max(Vector3 a, Vector3 b) {
	return (Vector3){
		.x = MAX(a.x, b.x),
		.y = MAX(a.y, b.y),
		.z = MAX(a.z, b.z)
	};
}

static inline real vector3_max_axis(Vector3 v) {
	real max_xy = v.x < v.y ? v.y : v.x;
	return max_xy < v.z ? v.z : max_xy;
}

static inline int vector3_max_axis_index(Vector3 v) {
	if (v.x < v.y) {
		return v.y < v.z ? 2 : 1;
	} else {
		return v.x < v.z ? 2 : 0;
	}
}

static inline bool vector3_is_normalized(Vector3 v) {
	return ABS(vector3_length2(v) - 1.0) < VECTOR3_EPSILON;
}

static inline real vector3_angle_to(Vector3 a, Vector3 b) {
	if (vector3_length2(a) *
		vector3_length2(b) == 0.0) return 0.0;
	return ATAN2(vector3_length(vector3_cross(a, b)), vector3_dot(a, b));
}

static inline Vector3 vector3_bounce(Vector3 v, Vector3 normal) {
	return vector3_subtract(v,
		vector3_scale(normal, 2.0 * vector3_dot(v, normal)));
}

static inline Vector3 vector3_reflect(Vector3 v, Vector3 normal) {
	return vector3_add(v,
		vector3_scale(normal, 2.0 * vector3_dot(v, normal)));
}

static inline Vector3 vector3_slide(Vector3 v, Vector3 normal) {
	return vector3_subtract(v,
		vector3_scale(normal, vector3_dot(v, normal)));
}

static inline Vector3 vector3_limit_length(Vector3 v, real max_length) {
	real l = vector3_length(v);
	if (l > max_length && l > 0.0) {
		return vector3_scale(v, max_length / l);
	} else return v;
}

static inline Vector3 vector3_clamp(Vector3 v, Vector3 min, Vector3 max) {
	return (Vector3){
		.x = CLAMP(v.x, min.x, max.x),
		.y = CLAMP(v.y, min.y, max.y),
		.z = CLAMP(v.z, min.z, max.z)
	};
}

static inline Vector3 vector3_sign(Vector3 v) {
	return (Vector3){
		.x = v.x > 0.0 ? 1.0 : ((v.x < 0.0) ? -1.0 : 0.0),
		.y = v.y > 0.0 ? 1.0 : ((v.y < 0.0) ? -1.0 : 0.0),
		.z = v.z > 0.0 ? 1.0 : ((v.z < 0.0) ? -1.0 : 0.0)
	};
}

static inline Vector3 vector3_snap(Vector3 v, Vector3 step) {
	return (Vector3){
		.x = (step.x != 0.0) ? ROUND(v.x / step.x) * step.x : v.x,
		.y = (step.y != 0.0) ? ROUND(v.y / step.y) * step.y : v.y,
		.z = (step.z != 0.0) ? ROUND(v.z / step.z) * step.z : v.z
	};
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

#endif /* MATH_VECTOR3_H */