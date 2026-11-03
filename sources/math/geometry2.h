#ifndef MATH_GEOMETRY2_H
#define MATH_GEOMETRY2_H

#include "vector2.h"

static inline
bool geometry2_is_quad_convex(
	Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4)
{
	Vector2 p12 = vector2_direction(p1, p2);
	Vector2 p23 = vector2_direction(p2, p3);
	Vector2 p34 = vector2_direction(p3, p4);
	Vector2 p41 = vector2_direction(p4, p1);

	real c123 = vector2_cross(p12, p23);
	real c234 = vector2_cross(p23, p34);
	real c341 = vector2_cross(p34, p41);
	real c412 = vector2_cross(p41, p12);

	return
		(c123 > VECTOR2_EPSILON &&
		c234 > VECTOR2_EPSILON &&
		c341 > VECTOR2_EPSILON &&
		c412 > VECTOR2_EPSILON) ||
		(c123 < -VECTOR2_EPSILON &&
		c234 < -VECTOR2_EPSILON &&
		c341 < -VECTOR2_EPSILON &&
		c412 < -VECTOR2_EPSILON);
}

#endif /* MATH_GEOMETRY2_H */