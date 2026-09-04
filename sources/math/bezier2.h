#ifndef MATH_BEZIER2_H
#define MATH_BEZIER2_H

#include "vector2.h"
#include "aabb2.h"

#define BEZIER2_EPSILON _Generic((real)0, \
	float: 1e-5F, \
	long double: 1e-14L, \
	default: 1e-9 \
)

#define BEZIER2_INTERSECTION_TOLERANCE _Generic((real)0, \
	float: 1e-2F, \
	long double: 1e-4L, \
	default: 1e-3 \
)

#define BEZIER2_PARAMETRIC_TOLERANCE _Generic((real)0, \
	float: 1e-4F, \
	long double: 1e-8L, \
	default: 1e-6 \
)

#define BEZIER2_MAX_INTERSECTIONS 16
#define BEZIER2_MAX_VERTICES 2048

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
	Vector2 p;
	real t1, t2;
} Bezier2Intersection;

typedef struct Bezier2Intersections {
	Bezier2Intersection points[BEZIER2_MAX_INTERSECTIONS];
	int count;
} Bezier2Intersections;

typedef struct Bezier2Vertices {
	Vector2 points[BEZIER2_MAX_VERTICES];
	int count;
} Bezier2Vertices;

/* TODO: 2D Cubic Bezier curves are necessary for SVG triangulation */
/* They can be intersected better than lines, but the math is tricky */
/* I also plan to implement thickness based shifting for SVG paths */

#endif /* MATH_BEZIER2_H */