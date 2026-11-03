#ifndef DEFINITIONS_EQUALS_H
#define DEFINITIONS_EQUALS_H

#include "bool.h"
#include "integer.h"
#include "float.h"

static inline bool i8_equals(const void* a, const void* b) {
	return *(const i8*)a == *(const i8*)b;
}

static inline bool i16_equals(const void* a, const void* b) {
	return *(const i16*)a == *(const i16*)b;
}

static inline bool i32_equals(const void* a, const void* b) {
	return *(const i32*)a == *(const i32*)b;
}

static inline bool i64_equals(const void* a, const void* b) {
	return *(const i64*)a == *(const i64*)b;
}

static inline bool u8_equals(const void* a, const void* b) {
	return *(const u8*)a == *(const u8*)b;
}

static inline bool u16_equals(const void* a, const void* b) {
	return *(const u16*)a == *(const u16*)b;
}

static inline bool u32_equals(const void* a, const void* b) {
	return *(const u32*)a == *(const u32*)b;
}

static inline bool u64_equals(const void* a, const void* b) {
	return *(const u64*)a == *(const u64*)b;
}

static inline bool isize_equals(const void* a, const void* b) {
	return *(const isize*)a == *(const isize*)b;
}

static inline bool usize_equals(const void* a, const void* b) {
	return *(const usize*)a == *(const usize*)b;
}

static inline bool f32_equals(const void* a, const void* b) {
	return *(const f32*)a == *(const f32*)b;
}

static inline bool f64_equals(const void* a, const void* b) {
	return *(const f64*)a == *(const f64*)b;
}

static inline bool real_equals(const void* a, const void* b) {
	return *(const real*)a == *(const real*)b;
}

#endif /* DEFINITIONS_EQUALS_H */