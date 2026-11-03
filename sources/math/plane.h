#ifndef MATH_PLANE_H
#define MATH_PLANE_H

#include "vector3.h"

#define PLANE_EPSILON \
	VECTOR3_EPSILON

#define PLANE_INTERSECTION_TOLERANCE \
	SWITCH_BASED_ON_REAL_TYPE \
		(1e-3F, 1e-7, 1e-10L)

typedef struct Plane {
	union {
		struct { real x, y, z, d; };
		struct { Vector3 normal; real distance; };
		real components[4];
	};
} Plane;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Plane plane_zero(void) {
	Plane p;
	p.normal = vector3_zero();
	p.distance = 0.0;
	return p;
}

static inline
Plane plane_from_components(real x, real y, real z, real d) {
	Plane p;
	p.x = x;
	p.y = y;
	p.z = z;
	p.d = d;
	return p;
}

static inline Plane plane_from_normal(Vector3 normal) {
	Plane p;
	p.normal = normal;
	p.distance = 0.0;
	return p;
}

static inline Plane plane_from_direction(Vector3 direction) {
	Plane p;
	p.normal = vector3_normalize(direction);
	p.distance = 0.0;
	return p;
}

static inline
Plane plane_from_normal_at_distance(Vector3 normal, real distance) {
	Plane p;
	p.normal = normal;
	p.distance = distance;
	return p;
}

static inline
Plane plane_from_direction_at_distance(Vector3 direction, real distance) {
	Plane p;
	p.normal = vector3_normalize(direction);
	p.distance = distance;
	return p;
}

static inline
Plane plane_from_direction_at_point(Vector3 point, Vector3 direction) {
	Plane p;
	p.normal = vector3_normalize(direction);
	p.distance = vector3_dot(p.normal, point);
	return p;
}

static inline
Plane plane_from_normal_at_point(Vector3 point, Vector3 normal) {
	Plane p;
	p.normal = normal;
	p.distance = vector3_dot(normal, point);
	return p;
}

static inline
Plane plane_from_points_ccw(Vector3 p1, Vector3 p2, Vector3 p3) {
	return plane_from_direction_at_point(p1, vector3_cross(
		vector3_direction(p1, p2), vector3_direction(p1, p3)));
}

static inline
Plane plane_from_points_cw(Vector3 p1, Vector3 p2, Vector3 p3) {
	return plane_from_direction_at_point(p1, vector3_cross(
		vector3_direction(p1, p3), vector3_direction(p1, p2)));
}

static inline
Plane plane_from_points(Vector3 p1, Vector3 p2, Vector3 p3) {
	return plane_from_points_ccw(p1, p2, p3);
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline bool plane_is_degenerate(Plane p) {
	return vector3_is_zero(p.normal, PLANE_EPSILON);
}

static inline bool plane_is_normalized(Plane p) {
	return vector3_is_normalized(p.normal);
}

static inline Vector3 plane_get_center(Plane p) {
	return vector3_scale(p.normal, p.distance);
}

static inline bool plane_is_equal(Plane a, Plane b, real epsilon) {
	return
		vector3_is_equal(a.normal, b.normal, epsilon) &&
		ABS(a.distance - b.distance) < epsilon;
}

#define Plane_equals plane_equals
static inline bool plane_equals(Plane a, Plane b) {
	return plane_is_equal(a, b, PLANE_EPSILON);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline Plane plane_normalize(Plane p) {
	real l = vector3_length(p.normal);
	return (l == 0.0)
		? plane_zero()
		: plane_from_normal_at_distance(
			vector3_from_components(
				p.normal.x / l,
				p.normal.y / l,
				p.normal.z / l),
			p.distance / l);
}

static inline real plane_signed_distance_to_point(Plane p, Vector3 point) {
	return vector3_dot(p.normal, point) - p.distance;
}

static inline real plane_distance_to_point(Plane p, Vector3 point) {
	return ABS(plane_signed_distance_to_point(p, point));
}

static inline real plane_distance2_to_point(Plane p, Vector3 point) {
	real signed_distance = plane_signed_distance_to_point(p, point);
	return signed_distance * signed_distance;
}

static inline bool plane_has_point(Plane p, Vector3 point, real epsilon) {
	return plane_distance_to_point(p, point) < epsilon;
}

static inline bool plane_is_point_over(Plane p, Vector3 point) {
	return plane_signed_distance_to_point(p, point) > 0.0;
}

static inline Vector3 plane_project_point(Plane p, Vector3 point) {
	return vector3_subtract(point, vector3_scale(p.normal,
		plane_signed_distance_to_point(p, point)));
}

static inline Vector3 plane_mirror_point(Plane p, Vector3 point) {
	return vector3_subtract(point, vector3_scale(p.normal,
		2.0 * plane_signed_distance_to_point(p, point)));
}

/* ========================================================================= */
/* Intersections                                                             */
/* ========================================================================= */

static inline
bool plane_intersect3(Plane p1, Plane p2, Plane p3, Vector3* intersection) {
	Vector3 c23 = vector3_cross(p2.normal, p3.normal);
	Vector3 c31 = vector3_cross(p3.normal, p1.normal);
	Vector3 c12 = vector3_cross(p1.normal, p2.normal);

	real denominator = vector3_dot(p1.normal, c23);
	if (ABS(denominator) < PLANE_INTERSECTION_TOLERANCE) {
		return false;
	}

	*intersection = vector3_scale(
		vector3_add(vector3_add(
			vector3_scale(c23, p1.distance),
			vector3_scale(c31, p2.distance)),
			vector3_scale(c12, p3.distance)),
		1.0 / denominator);

	return true;
}

static inline
bool plane_intersect_ray(Plane p,
	Vector3 point, Vector3 direction, Vector3* intersection)
{
	real denominator = vector3_dot(p.normal, direction);
	if (ABS(denominator) < PLANE_INTERSECTION_TOLERANCE) {
		return false;
	}

	real t = -plane_signed_distance_to_point(p, point) / denominator;
	if (t < 0.0) return false;

	*intersection = vector3_add(point,
		vector3_scale(direction, t));

	return true;
}

static inline
bool plane_intersect_segment(Plane p,
	Vector3 p1, Vector3 p2, Vector3* intersection)
{
	Vector3 p12 = vector3_direction(p1, p2);
	real denominator = vector3_dot(p.normal, p12);
	if (ABS(denominator) < PLANE_INTERSECTION_TOLERANCE) {
		return false;
	}

	real t = -plane_signed_distance_to_point(p, p1) / denominator;
	if (t < 0.0 || t > 1.0) return false;

	*intersection = vector3_add(p1,
		vector3_scale(p12, t));

	return true;
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Plane_hash plane_hash
static inline usize plane_hash(Plane p) {
	return hash_real_mix4(p.x, p.y, p.z, p.d);
}

#endif /* MATH_PLANE_H */