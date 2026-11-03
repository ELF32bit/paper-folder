#ifndef MATH_BEZIER2_H
#define MATH_BEZIER2_H

#include "vector2.h"
#include "aabb2.h"

#define BEZIER2_EPSILON \
	VECTOR2_EPSILON

#define BEZIER2_INTERSECTION_TOLERANCE \
	SWITCH_BASED_ON_REAL_TYPE \
		(1e-2F, 1e-3, 1e-4L)

#define BEZIER2_PARAMETRIC_TOLERANCE \
	SWITCH_BASED_ON_REAL_TYPE \
		(1e-4F, 1e-6, 1e-8L)

#define BEZIER2_MAX_INTERSECTIONS 15
#define BEZIER2_MAX_INTERSECTIONS_CURVES \
	BEZIER2_MAX_INTERSECTIONS + 1

#define BEZIER2_MAX_VERTICES 256

typedef struct Bezier2 {
	union {
		struct { Vector2 p0, p1, p2, p3; };
		real components[8];
		Vector2 points[4];
	};
} Bezier2;

typedef enum Bezier2ExtendSide {
	BEZIER2_EXTEND_NONE = 0,
	BEZIER2_EXTEND_START = 1,
	BEZIER2_EXTEND_END = 2,
	BEZIER2_EXTEND_BOTH = 3,
} Bezier2ExtendSide;

typedef struct Bezier2Intersection {
	union {
		Vector2 point;
		Vector2 p;
	};
	real t1, t2;
} Bezier2Intersection;

typedef struct Bezier2Intersections {
	Bezier2Intersection points[BEZIER2_MAX_INTERSECTIONS];
	union {
		int count;
		int size;
	};
	bool is_sorted;
} Bezier2Intersections;

typedef struct Bezier2Vertices {
	Vector2 points[BEZIER2_MAX_VERTICES];
	union {
		int count;
		int size;
	};
} Bezier2Vertices;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Bezier2 bezier2_zero(void) {
	Bezier2 c;
	c.p0 = vector2_zero();
	c.p1 = vector2_zero();
	c.p2 = vector2_zero();
	c.p3 = vector2_zero();
	return c;
}

static inline
Bezier2 bezier2_from_components(
	Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3)
{
	Bezier2 c;
	c.p0 = p0;
	c.p1 = p1;
	c.p2 = p2;
	c.p3 = p3;
	return c;
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline bool bezier2_is_point(Bezier2 c) {
	Vector2 p01 = vector2_direction(c.p0, c.p1);
	Vector2 p02 = vector2_direction(c.p0, c.p2);
	Vector2 p03 = vector2_direction(c.p0, c.p3);

	real max_offset = 0.0;
	max_offset = MAX(max_offset, vector2_max_axis(vector2_abs(p01)));
	max_offset = MAX(max_offset, vector2_max_axis(vector2_abs(p02)));
	max_offset = MAX(max_offset, vector2_max_axis(vector2_abs(p03)));

	real scale = (max_offset > 1.0) ? max_offset : 1.0;
	real epsilon = scale * BEZIER2_EPSILON;
	real epsilon2 = epsilon * epsilon;

	real L01 = vector2_length2(p01);
	real L02 = vector2_length2(p02);
	real L03 = vector2_length2(p03);

	return
		(L01 <= epsilon2) &&
		(L02 <= epsilon2) &&
		(L03 <= epsilon2);
}

static inline bool bezier2_is_line(Bezier2 c) {
	Vector2 p01 = vector2_direction(c.p0, c.p1);
	Vector2 p02 = vector2_direction(c.p0, c.p2);
	Vector2 p03 = vector2_direction(c.p0, c.p3);

	real max = 0.0;
	max = MAX(max, vector2_max_axis(vector2_abs(p01)));
	max = MAX(max, vector2_max_axis(vector2_abs(p02)));
	max = MAX(max, vector2_max_axis(vector2_abs(p03)));

	real scale = (max > 1.0) ? max : 1.0;
	real epsilon = scale * BEZIER2_EPSILON;

	real L03 = vector2_length2(p03);
	if (L03 <= epsilon * epsilon) {
		return false;
	}

	real h1 = ABS(vector2_cross(p01, p03));
	real h2 = ABS(vector2_cross(p02, p03));

	epsilon *= SQRT(L03);
	if (h1 > epsilon || h2 > epsilon) {
		return false;
	}

	real t1 = vector2_dot(p01, p03);
	real t2 = vector2_dot(p02, p03);

	if (t1 < -epsilon || t1 > L03 + epsilon ||
		t2 < -epsilon || t2 > L03 + epsilon) {
		return false;
	}

	return true;
}

static inline bool bezier2_is_degenerate(Bezier2 c) {
	Vector2 p01 = vector2_direction(c.p0, c.p1);
	Vector2 p02 = vector2_direction(c.p0, c.p2);
	Vector2 p03 = vector2_direction(c.p0, c.p3);

	real max = 0.0;
	max = MAX(max, vector2_max_axis(vector2_abs(p01)));
	max = MAX(max, vector2_max_axis(vector2_abs(p02)));
	max = MAX(max, vector2_max_axis(vector2_abs(p03)));

	real scale = (max > 1.0) ? max : 1.0;
	real epsilon = scale * BEZIER2_EPSILON;
	epsilon *= epsilon;

	real L01 = vector2_length2(p01);
	real L02 = vector2_length2(p02);
	real L03 = vector2_length2(p03);

	if (L01 < epsilon &&
		L02 < epsilon &&
		L03 < epsilon) {
		return true;
	}

	real L12 = vector2_distance2(c.p1, c.p2);
	real L23 = vector2_distance2(c.p2, c.p3);

	if (L01 < epsilon ||
		L12 < epsilon ||
		L23 < epsilon) {
		return true;
	}

	return false;
}

static inline Vector2 bezier2_interpolate(Bezier2 c, real t) {
	Vector2 a = vector2_lerp(c.p0, c.p1, t);
	Vector2 b = vector2_lerp(c.p1, c.p2, t);
	Vector2 C = vector2_lerp(c.p2, c.p3, t);

	Vector2 d = vector2_lerp(a, b, t);
	Vector2 e = vector2_lerp(b, C, t);

	return vector2_lerp(d, e, t);
}

static inline Vector2 bezier2_derivative(Bezier2 c, real t) {
	Vector2 p01 = vector2_direction(c.p0, c.p1);
	Vector2 p12 = vector2_direction(c.p1, c.p2);
	Vector2 p23 = vector2_direction(c.p2, c.p3);

	real u = 1.0 - t;
	real wa = 3.0 * u * u;
	real wb = 6.0 * u * t;
	real wc = 3.0 * t * t;

	return vector2_from_components(
		(p01.x * wa) + (p12.x * wb) + (p23.x * wc),
		(p01.y * wa) + (p12.y * wb) + (p23.y * wc));
}

static inline Vector2 bezier2_second_derivative(Bezier2 c, real t) {
	Vector2 p01 = vector2_direction(c.p0, c.p1);
	Vector2 p12 = vector2_direction(c.p1, c.p2);
	Vector2 p23 = vector2_direction(c.p2, c.p3);

	Vector2 a = vector2_subtract(p12, p01);
	Vector2 b = vector2_subtract(p23, p12);

	real u = 1.0 - t;
	real wa = 6.0 * u;
	real wb = 6.0 * t;

	return vector2_from_components(
		(a.x * wa) + (b.x * wb),
		(a.y * wa) + (b.y * wb));
}

static inline AABB2 bezier2_get_approximate_aabb(Bezier2 c) {
	return aabb2_from_components(
		vector2_min(
			vector2_min(c.p0, c.p1),
			vector2_min(c.p2, c.p3)),
		vector2_max(
			vector2_max(c.p0, c.p1),
			vector2_max(c.p2, c.p3)));
}

static inline AABB2 bezier2_get_aabb(Bezier2 c) {
	Vector2 min = vector2_min(c.p0, c.p3);
	Vector2 max = vector2_max(c.p0, c.p3);

	if (bezier2_is_line(c)) {
		return aabb2_from_components(min, max);
	}

	FOR_EACH(axis, 2) {
		real l01 = c.p1.components[axis] - c.p0.components[axis];
		real l12 = c.p2.components[axis] - c.p1.components[axis];
		real l23 = c.p3.components[axis] - c.p2.components[axis];

		real a = 3.0 * (l23 - 2.0 * l12 + l01);
		real b = 6.0 * (l12 - l01);
		real C = 3.0 * l01;

		if (ABS(a) < BEZIER2_EPSILON) {
			if (ABS(b) > BEZIER2_EPSILON) {
				real t = -C / b;
				if (t > 0.0 && t < 1.0) {
					Vector2 p = bezier2_interpolate(c, t);
					min = vector2_min(min, p);
					max = vector2_max(max, p);
				}
			}
		} else {
			real discriminant = b * b - 4.0 * a * C;
			if (discriminant >= 0.0) {
				real d2 = SQRT(discriminant);
				real q = (b >= 0.0)
					? -0.5 * (b + d2)
					: -0.5 * (b - d2);

				real t1 = q / a;
				if (t1 > 0.0 && t1 < 1.0) {
					Vector2 p = bezier2_interpolate(c, t1);
					min = vector2_min(min, p);
					max = vector2_max(max, p);
				}

				if (ABS(q) > BEZIER2_EPSILON) {
					real t2 = C / q;
					if (t2 > 0.0 && t2 < 1.0) {
						Vector2 p = bezier2_interpolate(c, t2);
						min = vector2_min(min, p);
						max = vector2_max(max, p);
					}
				}
			}
		}
	}

	return aabb2_from_components(min, max);
}

static inline real bezier2_get_flatness2(Bezier2 c) {
	Vector2 ux = vector2_from_components(
		(3.0 * c.p1.x) - ((2.0 * c.p0.x) + c.p3.x),
		(3.0 * c.p1.y) - ((2.0 * c.p0.y) + c.p3.y));

	Vector2 uy = vector2_from_components(
		(3.0 * c.p2.x) - ((2.0 * c.p3.x) + c.p0.x),
		(3.0 * c.p2.y) - ((2.0 * c.p3.y) + c.p0.y));

	real dx = MAX(ABS(ux.x), ABS(uy.x));
	real dy = MAX(ABS(ux.y), ABS(uy.y));

	return ((dx * dx) + (dy * dy)) * 0.0625;
}

static inline real bezier2_get_flatness(Bezier2 c) {
	return SQRT(bezier2_get_flatness2(c));
}

static inline real bezier2_get_length_at_time(Bezier2 c, real t) {
	if (t <= 0.0) return 0.0;

	Vector2 v0 = bezier2_derivative(c, REAL(0.046910077030668L) * t);
	Vector2 v1 = bezier2_derivative(c, REAL(0.230765344947158L) * t);
	Vector2 v2 = bezier2_derivative(c, REAL(0.500000000000000L) * t);
	Vector2 v3 = bezier2_derivative(c, REAL(0.769234655052841L) * t);
	Vector2 v4 = bezier2_derivative(c, REAL(0.953089922969331L) * t);

	real l0 = vector2_length(v0) * REAL(0.118463442528095L);
	real l1 = vector2_length(v1) * REAL(0.239314335249683L);
	real l2 = vector2_length(v2) * REAL(0.284444444444444L);
	real l3 = vector2_length(v3) * REAL(0.239314335249683L);
	real l4 = vector2_length(v4) * REAL(0.118463442528095L);

	return (l0 + l1 + l2 + l3 + l4) * t;
}

static inline
real bezier2_get_time_at_length(Bezier2 c, real c_length, real length) {
	if (length >= c_length) return 1.0;
	if (length <= 0.0) return 0.0;

	real t = length / c_length;
	FOR_EACH(i, SWITCH_BASED_ON_REAL_TYPE(5, 10, 20)) {
		real current_length = bezier2_get_length_at_time(c, t);

		real error = current_length - length;
		if (ABS(error) < BEZIER2_PARAMETRIC_TOLERANCE) break;

		real speed = vector2_length(bezier2_derivative(c, t));
		if (speed < BEZIER2_EPSILON) speed = BEZIER2_EPSILON;

		t -= error / speed;
		t = CLAMP(t, 0.0, 1.0);
	}

	return t;
}

static inline bool bezier2_is_equal(Bezier2 c1, Bezier2 c2) {
	Vector2 p01 = vector2_direction(c1.p0, c1.p1);
	Vector2 p02 = vector2_direction(c1.p0, c1.p2);
	Vector2 p03 = vector2_direction(c1.p0, c1.p3);

	real max = 0.0;
	max = MAX(max, vector2_max_axis(vector2_abs(p01)));
	max = MAX(max, vector2_max_axis(vector2_abs(p02)));
	max = MAX(max, vector2_max_axis(vector2_abs(p03)));

	real scale = (max > 1.0) ? max : 1.0;
	real epsilon = scale * BEZIER2_EPSILON;

	bool c12 = (
		vector2_is_equal(c1.p0, c2.p0, epsilon) &&
		vector2_is_equal(c1.p3, c2.p3, epsilon));

	bool c21 = (
		vector2_is_equal(c1.p0, c2.p3, epsilon) &&
		vector2_is_equal(c1.p3, c2.p0, epsilon));

	if NOT(c12 || c21) return false;

	if (c12 &&
		vector2_is_equal(c1.p1, c2.p1, epsilon) &&
		vector2_is_equal(c1.p2, c2.p2, epsilon)) {
		return true;
	}

	if (c21 &&
		vector2_is_equal(c1.p1, c2.p2, epsilon) &&
		vector2_is_equal(c1.p2, c2.p1, epsilon)) {
		return true;
	}

	return bezier2_is_line(c1) && bezier2_is_line(c2);
}

#define Bezier2_equals
static inline bool bezier2_equals(Bezier2 c1, Bezier2 c2) {
	return bezier2_is_equal(c1, c2);
}

/* ========================================================================= */
/* Splitting                                                                 */
/* ========================================================================= */

static inline
void bezier2_split(Bezier2 c, real t, Bezier2* left, Bezier2* right) {
	if (t <= 0.0) {
		left->p0 = c.p0;
		left->p1 = c.p0;
		left->p2 = c.p0;
		left->p3 = c.p0;
		*right = c;
		return;
	}

	if (t >= 1.0) {
		*left = c;
		right->p0 = c.p3;
		right->p1 = c.p3;
		right->p2 = c.p3;
		right->p3 = c.p3;
		return;
	}

	Vector2 a = vector2_lerp(c.p0, c.p1, t);
	Vector2 b = vector2_lerp(c.p1, c.p2, t);
	Vector2 C = vector2_lerp(c.p2, c.p3, t);

	Vector2 d = vector2_lerp(a, b, t);
	Vector2 e = vector2_lerp(b, C, t);

	Vector2 s = vector2_lerp(d, e, t);

	left->p0 = c.p0;
	left->p1 = a;
	left->p2 = d;
	left->p3 = s;

	right->p0 = s;
	right->p1 = e;
	right->p2 = C;
	right->p3 = c.p3;
}

static inline
void bezier2_split_range(Bezier2 c, real t1, real t2, Bezier2* middle) {
	t1 = CLAMP(t1, 0.0, 1.0);
	t2 = CLAMP(t2, 0.0, 1.0);
	if (t1 > t2) SWAP(t1, t2);

	if (t1 == 0.0 && t2 == 1.0) {
		*middle = c;
		return;
	}

	if (t2 == 0.0) {
		middle->p0 = c.p0;
		middle->p1 = c.p0;
		middle->p2 = c.p0;
		middle->p3 = c.p0;
		return;
	}

	if (t1 == t2) {
		Vector2 p = bezier2_interpolate(c, t1);
		middle->p0 = p;
		middle->p1 = p;
		middle->p2 = p;
		middle->p3 = p;
		return;
	}

	Bezier2 left, right;
	bezier2_split(c, t2, &left, &right);
	if (t1 == 0.0) {
		*middle = left;
		return;
	}

	real t = (t2 > BEZIER2_EPSILON)
		? CLAMP(t1 / t2, 0.0, 1.0)
		: 0.0;

	Bezier2 _;
	bezier2_split(left, t, &_, middle);
}

/* ========================================================================= */
/* Extrapolation                                                             */
/* ========================================================================= */

static inline
void bezier2_stroke(Bezier2 c, real thickness, Bezier2* left, Bezier2* right) {
	Vector2 p01 = vector2_direction(c.p0, c.p1);
	Vector2 p12 = vector2_direction(c.p1, c.p2);
	Vector2 p23 = vector2_direction(c.p2, c.p3);
	real thickness2 = thickness * 0.5;

	Vector2 n01 = vector2_orthogonal(p01);
	Vector2 n12 = vector2_orthogonal(p12);
	Vector2 n23 = vector2_orthogonal(p23);
	Vector2 n03 = vector2_normalize(vector2_orthogonal(
		vector2_direction(c.p0, c.p3)));

	n01 = (vector2_length2(n01) > BEZIER2_EPSILON)
		? vector2_normalize(n01) : n03;
	n12 = (vector2_length2(n12) > BEZIER2_EPSILON)
		? vector2_normalize(n12) : n01;
	n23 = (vector2_length2(n23) > BEZIER2_EPSILON)
		? vector2_normalize(n23) : n12;

	Vector2 p1n;
	if (vector2_dot(n01, n12) < 0.0) {
		p1n = n01;
	} else {
		Vector2 p1n_sum = vector2_add(n01, n12);
		p1n = (vector2_length2(p1n_sum) > BEZIER2_EPSILON)
			? vector2_normalize(p1n_sum) : n01;
	}

	Vector2 p2n;
	if (vector2_dot(n12, n23) < 0.0) {
		p2n = n12;
	} else {
		Vector2 p2n_sum = vector2_add(n12, n23);
		p2n = (vector2_length2(p2n_sum) > BEZIER2_EPSILON)
			? vector2_normalize(p2n_sum) : n12;
	}

	left->p0 = vector2_add(c.p0, vector2_scale(n01, thickness2));
	left->p1 = vector2_add(c.p1, vector2_scale(p1n, thickness2));
	left->p2 = vector2_add(c.p2, vector2_scale(p2n, thickness2));
	left->p3 = vector2_add(c.p3, vector2_scale(n23, thickness2));

	right->p0 = vector2_add(c.p0, vector2_scale(n01, -thickness2));
	right->p1 = vector2_add(c.p1, vector2_scale(p1n, -thickness2));
	right->p2 = vector2_add(c.p2, vector2_scale(p2n, -thickness2));
	right->p3 = vector2_add(c.p3, vector2_scale(n23, -thickness2));
}

static inline
Bezier2 bezier2_extend(Bezier2 c, real distance, Bezier2ExtendSide side) {
	if (side == BEZIER2_EXTEND_NONE) return c;
	if (distance <= 0.0) return c;

	Bezier2 extended = c;
	if (side & BEZIER2_EXTEND_START) {
		Vector2 p01 = vector2_direction(c.p0, c.p1);
		real l01 = vector2_length(p01);

		if (l01 < BEZIER2_EPSILON) {
			p01 = vector2_direction(c.p0, c.p3);
			l01 = vector2_length(p01);
		}

		Vector2 p01n = vector2_scale(p01, 1.0 / l01);
		extended.p0 = vector2_add(c.p0, vector2_scale(p01n, -distance));
		extended.p1 = vector2_add(c.p1, vector2_scale(p01n, -distance));
	}

	if (side & BEZIER2_EXTEND_END) {
		Vector2 p23 = vector2_direction(c.p2, c.p3);
		real l23 = vector2_length(p23);

		if (l23 < BEZIER2_EPSILON) {
			p23 = vector2_direction(c.p0, c.p3);
			l23 = vector2_length(p23);
		}

		Vector2 p23n = vector2_scale(p23, 1.0 / l23);
		extended.p3 = vector2_add(c.p3, vector2_scale(p23n, distance));
		extended.p2 = vector2_add(c.p2, vector2_scale(p23n, distance));
	}

	return extended;
}

/* ========================================================================= */
/* Intersections                                                             */
/* ========================================================================= */

Bezier2Intersections bezier2_intersect(Bezier2 c1, Bezier2 c2);
void bezier2_sort_intersections(Bezier2Intersections* intersections, bool t12);

static inline
void bezier2_split_intersections(const Bezier2Intersections* intersections,
	Bezier2 c12, bool t12, Bezier2* curves, int* count)
{
	const Bezier2Intersection* points = intersections->points;
	ASSERT(intersections->is_sorted);

	real t_previous = 0.0;
	FOR_EACH(index, intersections->count + 1) {
		real t_current = 1.0;
		if (index < USIZE(intersections->count)) {
			t_current = NOT(t12) ? points[index].t1 : points[index].t2;
			if (t_current < BEZIER2_EPSILON) { t_current = 0.0; }
			else if (t_current > 1.0 - BEZIER2_EPSILON) {
				t_current = 1.0;
			}
		}
		if (t_current <= t_previous) continue;

		Bezier2 middle;
		bezier2_split_range(c12, t_previous, t_current, &middle);
		if NOT(bezier2_is_degenerate(middle)) {
			curves[*count] = middle;
			(*count)++;
		}

		t_previous = t_current;
	}
}

/* ========================================================================= */
/* Tessellation                                                              */
/* ========================================================================= */

static inline
Bezier2Vertices bezier2_tessellate_uniform(Bezier2 c,
	real max_segment_length)
{
	ASSERT(max_segment_length > BEZIER2_EPSILON);
	Bezier2Vertices vertices;
	vertices.count = 0;

	real c_length = bezier2_get_length_at_time(c, 1.0);
	int segments = (int)CEIL(c_length / max_segment_length);
	segments = CLAMP(segments, 2, BEZIER2_MAX_VERTICES - 1);

	vertices.points[vertices.count++] = c.p0;
	real step = c_length / REAL(segments);

	FOR_EACH_IN_RANGE(index, 1, segments) {
		real length = REAL(index) * step;
		real t = bezier2_get_time_at_length(c, c_length, length);
		vertices.points[vertices.count++] = bezier2_interpolate(c, t);
	}

	vertices.points[vertices.count++] = c.p3;
	return vertices;
}

Bezier2Vertices bezier2_tessellate_adaptive(Bezier2 c,
	real flatness, real min_segment_length);

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Bezier2_hash bezier2_hash
static inline usize bezier2_hash(Bezier2 c) {
	return hash_usize_mix4(
		vector2_hash(c.p0),
		vector2_hash(c.p1),
		vector2_hash(c.p2),
		vector2_hash(c.p3));
}

#endif /* MATH_BEZIER2_H */