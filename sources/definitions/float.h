#ifndef DEFINITIONS_FLOAT_H
#define DEFINITIONS_FLOAT_H

#include <float.h>
#include "assert.h"

typedef float f32;
#define F32_MIN FLT_MIN
#define F32_MAX FLT_MAX
#define F32_EPSILON FLT_EPSILON

static_assert(sizeof(f32) == 4, "sizeof(f32) != 4");

typedef double f64;
#define F64_MIN DBL_MIN
#define F64_MAX DBL_MAX
#define F64_EPSILON DBL_EPSILON

static_assert(sizeof(f64) == 8, "sizeof(f64) != 8");

typedef double real;

#define REAL_MIN _Generic((real)0, \
	float: FLT_MIN, \
	long double: LDBL_MIN, \
	default: DBL_MIN \
)

#define REAL_MAX _Generic((real)0, \
	float: FLT_MAX, \
	long double: LDBL_MAX, \
	default: DBL_MAX \
)

#define REAL_EPSILON _Generic((real)0, \
	float: FLT_EPSILON, \
	long double: LDBL_EPSILON, \
	default: DBL_EPSILON \
)

#endif /* DEFINITIONS_FLOAT_H */