#ifndef DEFINITIONS_MATH_H
#define DEFINITIONS_MATH_H

#include <math.h>

#ifdef F32_EPSILON
#define F32_INFINITY ((f32)INFINITY)
#define F32_NAN ((f32)NAN)
#endif

#ifdef F64_EPSILON
#define F64_INFINITY ((f64)INFINITY)
#define F64_NAN ((f64)NAN)
#endif

#ifdef REAL_EPSILON
#define REAL_INFINITY ((real)INFINITY)
#define REAL_NAN ((real)NAN)
#endif

/* ========================================================================= */
/* Trigonometric Functions                                                   */
/* ========================================================================= */

#define ACOS(x) _Generic((x), \
	float: acosf(x), \
	long double: acosl(x), \
	default: acos(x) \
)

#define ASIN(x) _Generic((x), \
	float: asinf(x), \
	long double: asinl(x), \
	default: asin(x) \
)

#define ATAN(x) _Generic((x), \
	float: atanf(x), \
	long double: atanl(x), \
	default: atan(x) \
)

#define ATAN2(y, x) _Generic((y) + (x), \
	float: atan2f((y), (x)), \
	long double: atan2l((y), (x)), \
	default: atan2((y), (x)) \
)

#define COS(x) _Generic((x), \
	float: cosf(x), \
	long double: cosl(x), \
	default: cos(x) \
)

#define SIN(x) _Generic((x), \
	float: sinf(x), \
	long double: sinl(x), \
	default: sin(x) \
)

#define TAN(x) _Generic((x), \
	float: tanf(x), \
	long double: tanl(x), \
	default: tan(x) \
)

/* ========================================================================= */
/* Hyperbolic Functions                                                      */
/* ========================================================================= */

#define ACOSH(x) _Generic((x), \
	float: acoshf(x), \
	long double: acoshl(x), \
	default: acosh(x) \
)

#define ASINH(x) _Generic((x), \
	float: asinhf(x), \
	long double: asinhl(x), \
	default: asinh(x) \
)

#define ATANH(x) _Generic((x), \
	float: atanhf(x), \
	long double: atanhl(x), \
	default: atanh(x) \
)

#define COSH(x) _Generic((x), \
	float: coshf(x), \
	long double: coshl(x), \
	default: cosh(x) \
)

#define SINH(x) _Generic((x), \
	float: sinhf(x), \
	long double: sinhl(x), \
	default: sinh(x) \
)

#define TANH(x) _Generic((x), \
	float: tanhf(x), \
	long double: tanhl(x), \
	default: tanh(x) \
)

/* ========================================================================= */
/* Exponential & Logarithmic Functions                                       */
/* ========================================================================= */

#define EXP(x) _Generic((x), \
	float: expf(x), \
	long double: expl(x), \
	default: exp(x) \
)

#define EXP2(x) _Generic((x), \
	float: exp2f(x), \
	long double: exp2l(x), \
	default: exp2(x) \
)

#define EXPM1(x) _Generic((x), \
	float: expm1f(x), \
	long double: expm1l(x), \
	default: expm1(x) \
)

#define ILOGB(x) _Generic((x), \
	float: ilogbf(x), \
	long double: ilogbl(x), \
	default: ilogb(x) \
)

#define LOG(x) _Generic((x), \
	float: logf(x), \
	long double: logl(x), \
	default: log(x) \
)

#define LOG10(x) _Generic((x), \
	float: log10f(x), \
	long double: log10l(x), \
	default: log10(x) \
)

#define LOG1P(x) _Generic((x), \
	float: log1pf(x), \
	long double: log1pl(x), \
	default: log1p(x) \
)

#define LOG2(x) _Generic((x), \
	float: log2f(x), \
	long double: log2l(x), \
	default: log2(x) \
)

#define LOGB(x) _Generic((x), \
	float: logbf(x), \
	long double: logbl(x), \
	default: logb(x) \
)

/* ========================================================================= */
/* Power & Absolute-Value Functions                                          */
/* ========================================================================= */

#define CBRT(x) _Generic((x), \
	float: cbrtf(x), \
	long double: cbrtl(x), \
	default: cbrt(x) \
)

#undef ABS
#define ABS(x) _Generic((x), \
	float: fabsf(x), \
	double: fabs(x), \
	long double: fabsl(x), \
	default: ((x) < 0 ? -(x) : (x)) \
)

#define HYPOT(x, y) _Generic((x) + (y), \
	float: hypotf((x), (y)), \
	long double: hypotl((x), (y)), \
	default: hypot((x), (y)) \
)

#define POW(base, exp) _Generic((base) + (exp), \
	float: powf((base), (exp)), \
	long double: powl((base), (exp)), \
	default: pow((base), (exp)) \
)

#define SQRT(x) _Generic((x), \
	float: sqrtf(x), \
	long double: sqrtl(x), \
	default: sqrt(x) \
)

/* ========================================================================= */
/* Error & Gamma Functions                                                   */
/* ========================================================================= */

#define ERF(x) _Generic((x), \
	float: erff(x), \
	long double: erfl(x), \
	default: erf(x) \
)

#define ERFC(x) _Generic((x), \
	float: erfcf(x), \
	long double: erfcl(x), \
	default: erfc(x) \
)

#define LGAMMA(x) _Generic((x), \
	float: lgammaf(x), \
	long double: lgammal(x), \
	default: lgamma(x) \
)

#define TGAMMA(x) _Generic((x), \
	float: tgammaf(x), \
	long double: tgammal(x), \
	default: tgamma(x) \
)

/* ========================================================================= */
/* Nearest Integer Functions                                                 */
/* ========================================================================= */

#define CEIL(x) _Generic((x), \
	float: ceilf(x), \
	long double: ceill(x), \
	default: ceil(x) \
)

#define FLOOR(x) _Generic((x), \
	float: floorf(x), \
	long double: floorl(x), \
	default: floor(x) \
)

#define NEARBYINT(x) _Generic((x), \
	float: nearbyintf(x), \
	long double: nearbyintl(x), \
	default: nearbyint(x) \
)

#define RINT(x) _Generic((x), \
	float: rintf(x), \
	long double: rintl(x), \
	default: rint(x) \
)

#define ROUND(x) _Generic((x), \
	float: roundf(x), \
	long double: roundl(x), \
	default: round(x) \
)

#define TRUNC(x) _Generic((x), \
	float: truncf(x), \
	long double: truncl(x), \
	default: trunc(x) \
)

/* ========================================================================= */
/* Remainder Functions                                                       */
/* ========================================================================= */

#define FMOD(x, y) _Generic((x) + (y), \
	float: fmodf((x), (y)), \
	long double: fmodl((x), (y)), \
	default: fmod((x), (y)) \
)

#define REMAINDER(x, y) _Generic((x) + (y), \
	float: remainderf((x), (y)), \
	long double: remainderl((x), (y)), \
	default: remainder((x), (y)) \
)

/* ========================================================================= */
/* Manipulation Functions                                                    */
/* ========================================================================= */

#define COPYSIGN(x, y) _Generic((x) + (y), \
	float: copysignf((x), (y)), \
	long double: copysignl((x), (y)), \
	default: copysign((x), (y)) \
)

#define NEXTAFTER(x, y) _Generic((x) + (y), \
	float: nextafterf((x), (y)), \
	long double: nextafterl((x), (y)), \
	default: nextafter((x), (y)) \
)

/* ========================================================================= */
/* Minimum, Maximum, & Positive Difference Functions                         */
/* ========================================================================= */

#define FDIM(x, y) _Generic((x) + (y), \
	float: fdimf((x), (y)), \
	long double: fdiml((x), (y)), \
	default: fdim((x), (y)) \
)

#undef MIN
#define MIN(a, b) _Generic((a) + (b), \
	float: fminf((a), (b)), \
	double: fmin((a), (b)), \
	long double: fminl((a), (b)), \
	default: ((a) < (b) ? (a) : (b)) \
)

#undef MAX
#define MAX(a, b) _Generic((a) + (b), \
	float: fmaxf((a), (b)), \
	double: fmax((a), (b)), \
	long double: fmaxl((a), (b)), \
	default: ((a) > (b) ? (a) : (b)) \
)

/* ========================================================================= */
/* Floating Multiply-Add                                                     */
/* ========================================================================= */

#define FMA(x, y, z) _Generic((x) + (y) + (z), \
	float: fmaf((x), (y), (z)), \
	long double: fmal((x), (y), (z)), \
	default: fma((x), (y), (z)) \
)

#endif /* DEFINITIONS_MATH_H */