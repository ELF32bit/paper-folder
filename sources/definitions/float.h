#ifndef DEFINITION_FLOAT_H
#define DEFINITION_FLOAT_H

#include <float.h>
#include <assert.h>

typedef float f32;
#define F32(x) ((f32)(x))
#define F32_MIN FLT_MIN
#define F32_MAX FLT_MAX
#define F32_EPSILON FLT_EPSILON

static_assert(sizeof(f32) == 4, "sizeof(f32) != 4");

typedef double f64;
#define F64(x) ((f64)(x))
#define F64_MIN DBL_MIN
#define F64_MAX DBL_MAX
#define F64_EPSILON DBL_EPSILON

static_assert(sizeof(f64) == 8, "sizeof(f64) != 8");

#ifndef REAL_TYPE
#define REAL_TYPE double
#endif

typedef REAL_TYPE real;
#define REAL(x) ((real)(x))

#define SWITCH_BASED_ON_REAL_TYPE(FLOAT, DOUBLE, LONG) \
	_Generic(REAL(0), \
		float: (FLOAT), \
		long double: (LONG), \
		default: (DOUBLE))

#define REAL_MIN \
	SWITCH_BASED_ON_REAL_TYPE \
		(FLT_MIN, DBL_MIN, LDBL_MIN)

#define REAL_MAX \
	SWITCH_BASED_ON_REAL_TYPE \
		(FLT_MAX, DBL_MAX, LDBL_MAX)

#define REAL_EPSILON \
	SWITCH_BASED_ON_REAL_TYPE \
		(FLT_EPSILON, DBL_EPSILON, LDBL_EPSILON)

#endif /* DEFINITION_FLOAT_H */