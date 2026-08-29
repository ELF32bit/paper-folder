#ifndef MATH_MATRIX3X3_H
#define MATH_MATRIX3X3_H

#include "vector3.h"

typedef struct Matrix3x3 {
	union {
		Vector3 rows[3];
		real elements[3][3];
		real components[9];
	};
} Matrix3x3;

static inline Matrix3x3 matrix3x3_zero(void) {
	return (Matrix3x3){
		.elements = {
			{0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0}
		}
	};
}

static inline Matrix3x3 matrix3x3_identity(void) {
	return (Matrix3x3){
		.elements = {
			{1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
			{0.0, 0.0, 1.0}
		}
	};
}

static inline Vector3 matrix3x3_transform(Matrix3x3 m, Vector3 v) {
	return (Vector3){
		.x = vector3_dot(m.rows[0], v),
		.y = vector3_dot(m.rows[1], v),
		.z = vector3_dot(m.rows[2], v)
	};
}

static inline Matrix3x3 matrix3x3_multiply(Matrix3x3 a, Matrix3x3 b) {
	Matrix3x3 result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.elements[i][j] =
				a.elements[i][0] * b.elements[0][j] +
				a.elements[i][1] * b.elements[1][j] +
				a.elements[i][2] * b.elements[2][j];
		}
	}
	return result;
}

static inline Vector3 matrix3x3_get_column(Matrix3x3 m, int column) {
	return (Vector3){
		.x = m.elements[0][column],
		.y = m.elements[1][column],
		.z = m.elements[2][column]
	};
}

static inline Matrix3x3 matrix3x3_set_column(Matrix3x3 m, int column, Vector3 v) {
	m.elements[0][column] = v.x;
	m.elements[1][column] = v.y;
	m.elements[2][column] = v.z;
	return m;
}

static inline Matrix3x3 matrix3x3_transpose(Matrix3x3 m) {
	return (Matrix3x3){
		.elements = {
			{ m.elements[0][0], m.elements[1][0], m.elements[2][0] },
			{ m.elements[0][1], m.elements[1][1], m.elements[2][1] },
			{ m.elements[0][2], m.elements[1][2], m.elements[2][2] }
		}
	};
}

static inline real matrix3x3_determinant(Matrix3x3 m) {
	return
		m.elements[0][0] *
			(m.elements[1][1] * m.elements[2][2] -
			m.elements[1][2] * m.elements[2][1]) -
		m.elements[0][1] *
			(m.elements[1][0] * m.elements[2][2] -
			m.elements[1][2] * m.elements[2][0]) +
		m.elements[0][2] *
			(m.elements[1][0] * m.elements[2][1] -
			m.elements[1][1] * m.elements[2][0]);
}

static inline bool matrix3x3_is_equal(Matrix3x3 a, Matrix3x3 b, real epsilon) {
	return
		vector3_is_equal(a.rows[0], b.rows[0], epsilon) &&
		vector3_is_equal(a.rows[1], b.rows[1], epsilon) &&
		vector3_is_equal(a.rows[2], b.rows[2], epsilon);
}

static inline bool matrix3x3_equals(Matrix3x3 a, Matrix3x3 b) {
	return matrix3x3_is_equal(a, b, VECTOR3_EPSILON);
}

#endif /* MATH_MATRIX3X3_H */