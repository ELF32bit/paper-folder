#include "bezier2.h"

#include <stdlib.h>

/* ========================================================================= */
/* Intersections                                                             */
/* ========================================================================= */

#define BEZIER2_MAX_INTERSECTIONS_DEPTH \
	SWITCH_BASED_ON_REAL_TYPE \
		(16, 32, 64)

static inline
void _bezier2_intersection_point_found(Bezier2Intersections* intersections,
	Bezier2 C1, Bezier2 C2, real t1_mid, real t2_mid)
{
	if (intersections->count >= BEZIER2_MAX_INTERSECTIONS) return;
	Bezier2Intersection* points = intersections->points;
	int count = intersections->count;

	Vector2 p1 = bezier2_interpolate(C1, t1_mid);
	Vector2 p2 = bezier2_interpolate(C2, t2_mid);
	Vector2 p_mid = vector2_scale(vector2_add(p1, p2), 0.5);

	FOR_EACH(index, count) {
		if (vector2_is_equal(points[index].p, p_mid,
				BEZIER2_INTERSECTION_TOLERANCE))
			return;
	}

	points[count].t1 = t1_mid;
	points[count].t2 = t2_mid;
	points[count].p = p_mid;
	intersections->count++;
}

static inline
void _bezier2_intersect_recursive(Bezier2Intersections* intersections,
	Bezier2 C1, Bezier2 c1, real t1_min, real t1_max,
	Bezier2 C2, Bezier2 c2, real t2_min, real t2_max,
	int depth)
{
	if (intersections->count >= BEZIER2_MAX_INTERSECTIONS) return;
	if (depth > BEZIER2_MAX_INTERSECTIONS_DEPTH) return;

	real t1_mid = (t1_min + t1_max) * 0.5;
	real t2_mid = (t2_min + t2_max) * 0.5;
	if ((t1_max - t1_min) < BEZIER2_PARAMETRIC_TOLERANCE &&
		(t2_max - t2_min) < BEZIER2_PARAMETRIC_TOLERANCE) {
		_bezier2_intersection_point_found(intersections,
			C1, C2, t1_mid, t2_mid);
		return;
	}

	AABB2 aabb1 = bezier2_get_approximate_aabb(c1);
	AABB2 aabb2 = bezier2_get_approximate_aabb(c2);
	if NOT(aabb2_intersects(aabb1, aabb2)) return;

	Vector2 size1 = aabb2_size(aabb1);
	bool c1_is_point = vector2_is_zero(size1,
		BEZIER2_INTERSECTION_TOLERANCE);

	Vector2 size2 = aabb2_size(aabb2);
	bool c2_is_point = vector2_is_zero(size2,
		BEZIER2_INTERSECTION_TOLERANCE);

	if (c1_is_point && c2_is_point) {
		_bezier2_intersection_point_found(intersections,
			C1, C2, t1_mid, t2_mid);
		return;
	} else if (c1_is_point) {
		Bezier2 c2_left, c2_right;
		bezier2_split(c2, 0.5, &c2_left, &c2_right);

		_bezier2_intersect_recursive(intersections,
			C1, c1, t1_min, t1_max,
			C2, c2_left, t2_min, t2_mid,
			depth + 1);

		_bezier2_intersect_recursive(intersections,
			C1, c1, t1_min, t1_max,
			C2, c2_right, t2_mid, t2_max,
			depth + 1);
		return;
	} else if (c2_is_point) {
		Bezier2 c1_left, c1_right;
		bezier2_split(c1, 0.5, &c1_left, &c1_right);

		_bezier2_intersect_recursive(intersections,
			C1, c1_left, t1_min, t1_mid,
			C2, c2, t2_min, t2_max,
			depth + 1);

		_bezier2_intersect_recursive(intersections,
			C1, c1_right, t1_mid, t1_max,
			C2, c2, t2_min, t2_max,
			depth + 1);
		return;
	}

	real area1 = aabb2_area(aabb1);
	real area2 = aabb2_area(aabb2);

	if (area1 >= area2) {
		Bezier2 c1_left, c1_right;
		bezier2_split(c1, 0.5, &c1_left, &c1_right);

		_bezier2_intersect_recursive(intersections,
			C1, c1_left, t1_min, t1_mid,
			C2, c2, t2_min, t2_max,
			depth + 1);

		_bezier2_intersect_recursive(intersections,
			C1, c1_right, t1_mid, t1_max,
			C2, c2, t2_min, t2_max,
			depth + 1);
	} else {
		Bezier2 c2_left, c2_right;
		bezier2_split(c2, 0.5, &c2_left, &c2_right);

		_bezier2_intersect_recursive(intersections,
			C1, c1, t1_min, t1_max,
			C2, c2_left, t2_min, t2_mid,
			depth + 1);

		_bezier2_intersect_recursive(intersections,
			C1, c1, t1_min, t1_max,
			C2, c2_right, t2_mid, t2_max,
			depth + 1);
	}
}

Bezier2Intersections bezier2_intersect(Bezier2 c1, Bezier2 c2) {
	Bezier2Intersections intersections;
	intersections.count = 0;
	intersections.is_sorted = false;
	_bezier2_intersect_recursive(&intersections,
		c1, c1, 0.0, 1.0, c2, c2, 0.0, 1.0, 1);
	return intersections;
}

static inline
int _bezier2_sort_c1_ascending(const void* a, const void* b) {
	real w1 = ((Bezier2Intersection*)a)->t1;
	real w2 = ((Bezier2Intersection*)b)->t1;
	if (w1 < w2) return -1;
	if (w1 > w2) return 1;
	return 0;
}

static inline
int _bezier2_sort_c2_ascending(const void* a, const void* b) {
	real w1 = ((Bezier2Intersection*)a)->t2;
	real w2 = ((Bezier2Intersection*)b)->t2;
	if (w1 < w2) return -1;
	if (w1 > w2) return 1;
	return 0;
}

void bezier2_sort_intersections(Bezier2Intersections* intersections, bool t12)
{
	intersections->is_sorted = true;
	if (intersections->count <= 1) return;
	qsort(intersections->points, intersections->count,
		sizeof(Bezier2Intersection), NOT(t12)
			? _bezier2_sort_c1_ascending
			: _bezier2_sort_c2_ascending);
}

/* ========================================================================= */
/* Tessellation                                                              */
/* ========================================================================= */

#define BEZIER2_MAX_TESSELLATION_DEPTH 32

static inline
real _bezier2_get_approximate_length2(Bezier2 curve) {
	real L01 = vector2_distance2(curve.p0, curve.p1);
	real L12 = vector2_distance2(curve.p1, curve.p2);
	real L23 = vector2_distance2(curve.p2, curve.p3);
	return L01 + L12 + L23;
}

static inline
void _bezier2_tessellate_adaptive_recursive(Bezier2 c,
	real flatness2, real min_segment_length2, Bezier2Vertices* vertices,
	int depth)
{
	if (vertices->count >= BEZIER2_MAX_VERTICES) return;
	if (depth >= BEZIER2_MAX_TESSELLATION_DEPTH ||
		bezier2_get_flatness2(c) <= flatness2 ||
		_bezier2_get_approximate_length2(c) <= min_segment_length2) {
		vertices->points[vertices->count++] = c.p3;
		return;
	}

	Bezier2 left, right;
	bezier2_split(c, 0.5, &left, &right);

	_bezier2_tessellate_adaptive_recursive(left,
		flatness2, min_segment_length2, vertices,
		depth + 1);

	_bezier2_tessellate_adaptive_recursive(right,
		flatness2, min_segment_length2, vertices,
		depth + 1);
}

Bezier2Vertices bezier2_tessellate_adaptive(Bezier2 c,
	real flatness, real min_segment_length)
{
	ASSERT(flatness >= 0.0);
	ASSERT(min_segment_length >= 0.0);
	Bezier2Vertices vertices;
	vertices.count = 0;

	vertices.points[vertices.count++] = c.p0;
	_bezier2_tessellate_adaptive_recursive(c, flatness * flatness,
		min_segment_length * min_segment_length,
		&vertices, 1);

	return vertices;
}