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
	float: acosf((float)(x)), \
	long double: acosl((long double)(x)), \
	default: acos(x) \
)

#define ASIN(x) _Generic((x), \
	float: asinf((float)(x)), \
	long double: asinl((long double)(x)), \
	default: asin(x) \
)

#define ATAN(x) _Generic((x), \
	float: atanf((float)(x)), \
	long double: atanl((long double)(x)), \
	default: atan(x) \
)

#define ATAN2(y, x) _Generic((y) + (x), \
	float: atan2f((float)(y), (float)(x)), \
	long double: atan2l((long double)(y), (long double)(x)), \
	default: atan2((y), (x)) \
)

#define COS(x) _Generic((x), \
	float: cosf((float)(x)), \
	long double: cosl((long double)(x)), \
	default: cos(x) \
)

#define SIN(x) _Generic((x), \
	float: sinf((float)(x)), \
	long double: sinl((long double)(x)), \
	default: sin(x) \
)

#define TAN(x) _Generic((x), \
	float: tanf((float)(x)), \
	long double: tanl((long double)(x)), \
	default: tan(x) \
)

/* ========================================================================= */
/* Hyperbolic Functions                                                      */
/* ========================================================================= */

#define ACOSH(x) _Generic((x), \
	float: acoshf((float)(x)), \
	long double: acoshl((long double)(x)), \
	default: acosh(x) \
)

#define ASINH(x) _Generic((x), \
	float: asinhf((float)(x)), \
	long double: asinhl((long double)(x)), \
	default: asinh(x) \
)

#define ATANH(x) _Generic((x), \
	float: atanhf((float)(x)), \
	long double: atanhl((long double)(x)), \
	default: atanh(x) \
)

#define COSH(x) _Generic((x), \
	float: coshf((float)(x)), \
	long double: coshl((long double)(x)), \
	default: cosh(x) \
)

#define SINH(x) _Generic((x), \
	float: sinhf((float)(x)), \
	long double: sinhl((long double)(x)), \
	default: sinh(x) \
)

#define TANH(x) _Generic((x), \
	float: tanhf((float)(x)), \
	long double: tanhl((long double)(x)), \
	default: tanh(x) \
)

/* ========================================================================= */
/* Exponential & Logarithmic Functions                                       */
/* ========================================================================= */

#define EXP(x) _Generic((x), \
	float: expf((float)(x)), \
	long double: expl((long double)(x)), \
	default: exp(x) \
)

#define EXP2(x) _Generic((x), \
	float: exp2f((float)(x)), \
	long double: exp2l((long double)(x)), \
	default: exp2(x) \
)

#define EXPM1(x) _Generic((x), \
	float: expm1f((float)(x)), \
	long double: expm1l((long double)(x)), \
	default: expm1(x) \
)

#define ILOGB(x) _Generic((x), \
	float: ilogbf((float)(x)), \
	long double: ilogbl((long double)(x)), \
	default: ilogb(x) \
)

#define LOG(x) _Generic((x), \
	float: logf((float)(x)), \
	long double: logl((long double)(x)), \
	default: log(x) \
)

#define LOG10(x) _Generic((x), \
	float: log10f((float)(x)), \
	long double: log10l((long double)(x)), \
	default: log10(x) \
)

#define LOG1P(x) _Generic((x), \
	float: log1pf((float)(x)), \
	long double: log1pl((long double)(x)), \
	default: log1p(x) \
)

#define LOG2(x) _Generic((x), \
	float: log2f((float)(x)), \
	long double: log2l((long double)(x)), \
	default: log2(x) \
)

#define LOGB(x) _Generic((x), \
	float: logbf((float)(x)), \
	long double: logbl((long double)(x)), \
	default: logb(x) \
)

/* ========================================================================= */
/* Power & Absolute-Value Functions                                          */
/* ========================================================================= */

#define CBRT(x) _Generic((x), \
	float: cbrtf((float)(x)), \
	long double: cbrtl((long double)(x)), \
	default: cbrt(x) \
)

#undef ABS
#define ABS(x) _Generic((x), \
	float: fabsf((float)(x)), \
	double: fabs((double)(x)), \
	long double: fabsl((long double)(x)), \
	default: ((x) < 0 ? -(x) : (x)) \
)

#define HYPOT(x, y) _Generic((x) + (y), \
	float: hypotf((float)(x), (float)(y)), \
	long double: hypotl((long double)(x), (long double)(y)), \
	default: hypot((x), (y)) \
)

#define POW(base, exp) _Generic((base) + (exp), \
	float: powf((float)(base), (float)(exp)), \
	long double: powl((long double)(base), (long double)(exp)), \
	default: pow((base), (exp)) \
)

#define SQRT(x) _Generic((x), \
	float: sqrtf((float)(x)), \
	long double: sqrtl((long double)(x)), \
	default: sqrt(x) \
)

/* ========================================================================= */
/* Error & Gamma Functions                                                   */
/* ========================================================================= */

#define ERF(x) _Generic((x), \
	float: erff((float)(x)), \
	long double: erfl((long double)(x)), \
	default: erf(x) \
)

#define ERFC(x) _Generic((x), \
	float: erfcf((float)(x)), \
	long double: erfcl((long double)(x)), \
	default: erfc(x) \
)

#define LGAMMA(x) _Generic((x), \
	float: lgammaf((float)(x)), \
	long double: lgammal((long double)(x)), \
	default: lgamma(x) \
)

#define TGAMMA(x) _Generic((x), \
	float: tgammaf((float)(x)), \
	long double: tgammal((long double)(x)), \
	default: tgamma(x) \
)

/* ========================================================================= */
/* Nearest Integer Functions                                                 */
/* ========================================================================= */

#define CEIL(x) _Generic((x), \
	float: ceilf((float)(x)), \
	long double: ceill((long double)(x)), \
	default: ceil(x) \
)

#define FLOOR(x) _Generic((x), \
	float: floorf((float)(x)), \
	long double: floorl((long double)(x)), \
	default: floor(x) \
)

#define NEARBYINT(x) _Generic((x), \
	float: nearbyintf((float)(x)), \
	long double: nearbyintl((long double)(x)), \
	default: nearbyint(x) \
)

#define RINT(x) _Generic((x), \
	float: rintf((float)(x)), \
	long double: rintl((long double)(x)), \
	default: rint(x) \
)

#define ROUND(x) _Generic((x), \
	float: roundf((float)(x)), \
	long double: roundl((long double)(x)), \
	default: round(x) \
)

#define TRUNC(x) _Generic((x), \
	float: truncf((float)(x)), \
	long double: truncl((long double)(x)), \
	default: trunc(x) \
)

/* ========================================================================= */
/* Remainder Functions                                                       */
/* ========================================================================= */

#define FMOD(x, y) _Generic((x) + (y), \
	float: fmodf((float)(x), (float)(y)), \
	long double: fmodl((long double)(x), (long double)(y)), \
	default: fmod((x), (y)) \
)

#define REMAINDER(x, y) _Generic((x) + (y), \
	float: remainderf((float)(x), (float)(y)), \
	long double: remainderl((long double)(x), (long double)(y)), \
	default: remainder((x), (y)) \
)

/* ========================================================================= */
/* Manipulation Functions                                                    */
/* ========================================================================= */

#define COPYSIGN(x, y) _Generic((x) + (y), \
	float: copysignf((float)(x), (float)(y)), \
	long double: copysignl((long double)(x), (long double)(y)), \
	default: copysign((x), (y)) \
)

#define NEXTAFTER(x, y) _Generic((x) + (y), \
	float: nextafterf((float)(x), (float)(y)), \
	long double: nextafterl((long double)(x), (long double)(y)), \
	default: nextafter((x), (y)) \
)

/* ========================================================================= */
/* Minimum, Maximum, & Positive Difference Functions                         */
/* ========================================================================= */

#define FDIM(x, y) _Generic((x) + (y), \
	float: fdimf((float)(x), (float)(y)), \
	long double: fdiml((long double)(x), (long double)(y)), \
	default: fdim((x), (y)) \
)

#undef MIN
#define MIN(a, b) _Generic((a) + (b), \
	float: fminf((float)(a), (float)(b)), \
	double: fmin((double)(a), (double)(b)), \
	long double: fminl((long double)(a), (long double)(b)), \
	default: ((a) < (b) ? (a) : (b)) \
)

#undef MAX
#define MAX(a, b) _Generic((a) + (b), \
	float: fmaxf((float)(a), (float)(b)), \
	double: fmax((double)(a), (double)(b)), \
	long double: fmaxl((long double)(a), (long double)(b)), \
	default: ((a) > (b) ? (a) : (b)) \
)

/* ========================================================================= */
/* Floating Multiply-Add                                                     */
/* ========================================================================= */

#define FMA(x, y, z) _Generic((x) + (y) + (z), \
	float: fmaf((float)(x), (float)(y), (float)(z)), \
	long double: fmal((long double)(x), (long double)(y), (long double)(z)), \
	default: fma((x), (y), (z)) \
)

#endif /* DEFINITIONS_MATH_H */