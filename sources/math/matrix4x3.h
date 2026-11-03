#ifndef MATH_MATRIX4X3_H
#define MATH_MATRIX4X3_H

#include "matrix3x3.h"

#define MATRIX4X3_EPSILON \
	VECTOR3_EPSILON

typedef struct Matrix4x3 {
	union {
		struct { Vector3 x, y, z, o; };
		struct {
			Matrix3x3 basis;
			Vector3 origin;
		};
		struct {
			real m00, m01, m02;
			real m10, m11, m12;
			real m20, m21, m22;
			real m30, m31, m32;
		};
		Vector3 columns[4];
		real elements[4][3];
		real components[12];
	};
} Matrix4x3;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Matrix4x3 matrix4x3_zero(void) {
	Matrix4x3 m;
	m.x = vector3_zero();
	m.y = vector3_zero();
	m.z = vector3_zero();
	m.o = vector3_zero();
	return m;
}

static inline
Matrix4x3 matrix4x3_from_columns(
	Vector3 x, Vector3 y, Vector3 z, Vector3 o)
{
	Matrix4x3 m;
	m.x = x;
	m.y = y;
	m.z = z;
	m.o = o;
	return m;
}

static inline
Matrix4x3 matrix4x3_from_basis_and_origin(
	Matrix3x3 basis, Vector3 origin)
{
	Matrix4x3 m;
	m.basis = basis;
	m.origin = origin;
	return m;
}

static inline
Matrix4x3 matrix4x3_from_components(
	real m00, real m01, real m02,
	real m10, real m11, real m12,
	real m20, real m21, real m22,
	real m30, real m31, real m32)
{
	Matrix4x3 m;
	m.x = vector3_from_components(m00, m01, m02);
	m.y = vector3_from_components(m10, m11, m12);
	m.z = vector3_from_components(m20, m21, m22);
	m.o = vector3_from_components(m30, m31, m32);
	return m;
}

static inline Matrix4x3 matrix4x3_identity(void) {
	return matrix4x3_from_columns(
		vector3_from_components(1.0, 0.0, 0.0),
		vector3_from_components(0.0, 1.0, 0.0),
		vector3_from_components(0.0, 0.0, 1.0),
		vector3_from_components(0.0, 0.0, 0.0));
}

/* ========================================================================= */
/* Operators                                                                 */
/* ========================================================================= */

static inline Vector3 matrix4x3_multiply_vector3(Matrix4x3 a, Vector3 b) {
	return vector3_add(matrix3x3_multiply_vector3(a.basis, b), a.origin);
}

static inline Matrix4x3 matrix4x3_multiply(Matrix4x3 a, Matrix4x3 b) {
	return matrix4x3_from_basis_and_origin(
		matrix3x3_multiply(a.basis, b.basis),
		matrix4x3_multiply_vector3(a, b.origin));
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline Matrix4x3 matrix4x3_orthonormalize(Matrix4x3 m) {
	return matrix4x3_from_basis_and_origin(
		matrix3x3_orthonormalize(m.basis),
		m.origin);
}

static inline Matrix4x3 matrix4x3_affine_inverse(Matrix4x3 m) {
	Matrix3x3 basis = matrix3x3_inverse(m.basis);
	return matrix4x3_from_basis_and_origin(basis,
		matrix3x3_multiply_vector3(basis,
			vector3_scale(m.origin, -1.0)));
}

static inline Matrix4x3 matrix4x3_inverse(Matrix4x3 m) {
	Matrix3x3 basis = matrix3x3_transpose(m.basis);
	return matrix4x3_from_basis_and_origin(basis,
		matrix3x3_multiply_vector3(basis,
			vector3_scale(m.origin, -1.0)));
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline bool matrix4x3_is_orthonormal(Matrix4x3 m) {
	return matrix3x3_is_orthonormal(m.basis);
}

static inline bool matrix4x3_is_equal(Matrix4x3 a, Matrix4x3 b, real epsilon) {
	return
		matrix3x3_is_equal(a.basis, b.basis, epsilon) &&
		vector3_is_equal(a.origin, b.origin, epsilon);
}

#define Matrix4x3_equals matrix4x3_equals
static inline bool matrix4x3_equals(Matrix4x3 a, Matrix4x3 b) {
	return matrix4x3_is_equal(a, b, MATRIX4X3_EPSILON);
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Matrix4x3_hash matrix4x3_hash
static inline usize matrix4x3_hash(Matrix4x3 m) {
	return hash_usize_mix4(
		vector3_hash(m.x),
		vector3_hash(m.y),
		vector3_hash(m.z),
		vector3_hash(m.o));
}

#endif /* MATH_MATRIX4X3_H */