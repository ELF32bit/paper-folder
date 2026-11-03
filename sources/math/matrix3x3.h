#ifndef MATH_MATRIX3X3_H
#define MATH_MATRIX3X3_H

#include "vector3.h"

#define MATRIX3X3_EPSILON \
	VECTOR3_EPSILON

typedef struct Matrix3x3 {
	union {
		struct { Vector3 x, y, z; };
		struct {
			real m00, m01, m02;
			real m10, m11, m12;
			real m20, m21, m22;
		};
		Vector3 columns[3];
		real elements[3][3];
		real components[9];
	};
} Matrix3x3;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Matrix3x3 matrix3x3_zero(void) {
	Matrix3x3 m;
	m.x = vector3_zero();
	m.y = vector3_zero();
	m.z = vector3_zero();
	return m;
}

static inline
Matrix3x3 matrix3x3_from_columns(
	Vector3 x, Vector3 y, Vector3 z)
{
	Matrix3x3 m;
	m.x = x;
	m.y = y;
	m.z = z;
	return m;
}

static inline
Matrix3x3 matrix3x3_from_components(
	real m00, real m01, real m02,
	real m10, real m11, real m12,
	real m20, real m21, real m22)
{
	Matrix3x3 m;
	m.x = vector3_from_components(m00, m01, m02);
	m.y = vector3_from_components(m10, m11, m12);
	m.z = vector3_from_components(m20, m21, m22);
	return m;
}

static inline Matrix3x3 matrix3x3_identity(void) {
	return matrix3x3_from_columns(
		vector3_from_components(1.0, 0.0, 0.0),
		vector3_from_components(0.0, 1.0, 0.0),
		vector3_from_components(0.0, 0.0, 1.0));
}

/* ========================================================================= */
/* Operators                                                                 */
/* ========================================================================= */

static inline Vector3 matrix3x3_multiply_vector3(Matrix3x3 a, Vector3 b) {
	return vector3_from_components(
		a.m00 * b.x + a.m10 * b.y + a.m20 * b.z,
		a.m01 * b.x + a.m11 * b.y + a.m21 * b.z,
		a.m02 * b.x + a.m12 * b.y + a.m22 * b.z);
}

static inline Matrix3x3 matrix3x3_multiply(Matrix3x3 a, Matrix3x3 b) {
	return matrix3x3_from_components(
		a.m00 * b.m00 + a.m10 * b.m01 + a.m20 * b.m02,
		a.m01 * b.m00 + a.m11 * b.m01 + a.m21 * b.m02,
		a.m02 * b.m00 + a.m12 * b.m01 + a.m22 * b.m02,
		a.m00 * b.m10 + a.m10 * b.m11 + a.m20 * b.m12,
		a.m01 * b.m10 + a.m11 * b.m11 + a.m21 * b.m12,
		a.m02 * b.m10 + a.m12 * b.m11 + a.m22 * b.m12,
		a.m00 * b.m20 + a.m10 * b.m21 + a.m20 * b.m22,
		a.m01 * b.m20 + a.m11 * b.m21 + a.m21 * b.m22,
		a.m02 * b.m20 + a.m12 * b.m21 + a.m22 * b.m22);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline Matrix3x3 matrix3x3_orthonormalize(Matrix3x3 m) {
	Vector3 x = vector3_normalize(m.x);
	Vector3 y = vector3_normalize(vector3_subtract(m.y,
		vector3_scale(x, vector3_dot(m.y, x))));
	return matrix3x3_from_columns(x, y,
		vector3_cross(x, y));
}

static inline Matrix3x3 matrix3x3_transpose(Matrix3x3 m) {
	return matrix3x3_from_columns(
		vector3_from_components(m.m00, m.m10, m.m20),
		vector3_from_components(m.m01, m.m11, m.m21),
		vector3_from_components(m.m02, m.m12, m.m22));
}

static inline Matrix3x3 matrix3x3_inverse(Matrix3x3 m) {
	Vector3 x = vector3_cross(m.y, m.z);
	Vector3 y = vector3_cross(m.z, m.x);
	Vector3 z = vector3_cross(m.x, m.y);

	real determinant = vector3_dot(m.x, x);
	if (ABS(determinant) < MATRIX3X3_EPSILON)
		return matrix3x3_identity();

	determinant = 1.0 / determinant;
	Vector3 X = vector3_from_components(x.x, y.x, z.x);
	Vector3 Y = vector3_from_components(x.y, y.y, z.y);
	Vector3 Z = vector3_from_components(x.z, y.z, z.z);

	return matrix3x3_from_columns(
		vector3_scale(X, determinant),
		vector3_scale(Y, determinant),
		vector3_scale(Z, determinant));
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline real matrix3x3_get_determinant(Matrix3x3 m) {
	return vector3_dot(m.x, vector3_cross(m.y, m.z));
}

static inline bool matrix3x3_is_orthonormal(Matrix3x3 m) {
	real xx = vector3_dot(m.x, m.x);
	real yy = vector3_dot(m.y, m.y);
	real zz = vector3_dot(m.z, m.z);

	if (ABS(xx - 1.0) > MATRIX3X3_EPSILON ||
		ABS(yy - 1.0) > MATRIX3X3_EPSILON ||
		ABS(zz - 1.0) > MATRIX3X3_EPSILON) {
		return false;
	}

	real xy = vector3_dot(m.x, m.y);
	real xz = vector3_dot(m.x, m.z);
	real yz = vector3_dot(m.y, m.z);

	if (ABS(xy) > MATRIX3X3_EPSILON ||
		ABS(xz) > MATRIX3X3_EPSILON ||
		ABS(yz) > MATRIX3X3_EPSILON) {
		return false;
	}

	return true;
}

static inline bool matrix3x3_is_equal(Matrix3x3 a, Matrix3x3 b, real epsilon) {
	return
		vector3_is_equal(a.x, b.x, epsilon) &&
		vector3_is_equal(a.y, b.y, epsilon) &&
		vector3_is_equal(a.z, b.z, epsilon);
}

#define Matrix3x3_equals matrix3x3_equals
static inline bool matrix3x3_equals(Matrix3x3 a, Matrix3x3 b) {
	return matrix3x3_is_equal(a, b, MATRIX3X3_EPSILON);
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Matrix3x3_hash matrix3x3_hash
static inline usize matrix3x3_hash(Matrix3x3 m) {
	return hash_usize_mix3(
		vector3_hash(m.x),
		vector3_hash(m.y),
		vector3_hash(m.z));
}

#endif /* MATH_MATRIX3X3_H */