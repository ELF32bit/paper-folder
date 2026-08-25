#ifndef DEFINITIONS_SORT_H
#define DEFINITIONS_SORT_H

#include <stdlib.h>
#include "integer.h"
#include "float.h"

/* ========================================================================= */
/* I8                                                                        */
/* ========================================================================= */

static inline int i8_sort_ascending(const void* a, const void* b) {
	if (*(const i8*)a < *(const i8*)b) return -1;
	if (*(const i8*)a > *(const i8*)b) return 1;
	return 0;
}

static inline int i8_sort_descending(const void* a, const void* b) {
	if (*(const i8*)a > *(const i8*)b) return -1;
	if (*(const i8*)a < *(const i8*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* I16                                                                       */
/* ========================================================================= */

static inline int i16_sort_ascending(const void* a, const void* b) {
	if (*(const i16*)a < *(const i16*)b) return -1;
	if (*(const i16*)a > *(const i16*)b) return 1;
	return 0;
}

static inline int i16_sort_descending(const void* a, const void* b) {
	if (*(const i16*)a > *(const i16*)b) return -1;
	if (*(const i16*)a < *(const i16*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* I32                                                                       */
/* ========================================================================= */

static inline int i32_sort_ascending(const void* a, const void* b) {
	if (*(const i32*)a < *(const i32*)b) return -1;
	if (*(const i32*)a > *(const i32*)b) return 1;
	return 0;
}

static inline int i32_sort_descending(const void* a, const void* b) {
	if (*(const i32*)a > *(const i32*)b) return -1;
	if (*(const i32*)a < *(const i32*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* I64                                                                       */
/* ========================================================================= */

static inline int i64_sort_ascending(const void* a, const void* b) {
	if (*(const i64*)a < *(const i64*)b) return -1;
	if (*(const i64*)a > *(const i64*)b) return 1;
	return 0;
}

static inline int i64_sort_descending(const void* a, const void* b) {
	if (*(const i64*)a > *(const i64*)b) return -1;
	if (*(const i64*)a < *(const i64*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* ISIZE                                                                     */
/* ========================================================================= */

static inline int isize_sort_ascending(const void* a, const void* b) {
	if (*(const isize*)a < *(const isize*)b) return -1;
	if (*(const isize*)a > *(const isize*)b) return 1;
	return 0;
}

static inline int isize_sort_descending(const void* a, const void* b) {
	if (*(const isize*)a > *(const isize*)b) return -1;
	if (*(const isize*)a < *(const isize*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* U8                                                                        */
/* ========================================================================= */

static inline int u8_sort_ascending(const void* a, const void* b) {
	if (*(const u8*)a < *(const u8*)b) return -1;
	if (*(const u8*)a > *(const u8*)b) return 1;
	return 0;
}

static inline int u8_sort_descending(const void* a, const void* b) {
	if (*(const u8*)a > *(const u8*)b) return -1;
	if (*(const u8*)a < *(const u8*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* U16                                                                       */
/* ========================================================================= */

static inline int u16_sort_ascending(const void* a, const void* b) {
	if (*(const u16*)a < *(const u16*)b) return -1;
	if (*(const u16*)a > *(const u16*)b) return 1;
	return 0;
}

static inline int u16_sort_descending(const void* a, const void* b) {
	if (*(const u16*)a > *(const u16*)b) return -1;
	if (*(const u16*)a < *(const u16*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* U32                                                                       */
/* ========================================================================= */

static inline int u32_sort_ascending(const void* a, const void* b) {
	if (*(const u32*)a < *(const u32*)b) return -1;
	if (*(const u32*)a > *(const u32*)b) return 1;
	return 0;
}

static inline int u32_sort_descending(const void* a, const void* b) {
	if (*(const u32*)a > *(const u32*)b) return -1;
	if (*(const u32*)a < *(const u32*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* U64                                                                       */
/* ========================================================================= */

static inline int u64_sort_ascending(const void* a, const void* b) {
	if (*(const u64*)a < *(const u64*)b) return -1;
	if (*(const u64*)a > *(const u64*)b) return 1;
	return 0;
}

static inline int u64_sort_descending(const void* a, const void* b) {
	if (*(const u64*)a > *(const u64*)b) return -1;
	if (*(const u64*)a < *(const u64*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* USIZE                                                                     */
/* ========================================================================= */

static inline int usize_sort_ascending(const void* a, const void* b) {
	if (*(const usize*)a < *(const usize*)b) return -1;
	if (*(const usize*)a > *(const usize*)b) return 1;
	return 0;
}

static inline int usize_sort_descending(const void* a, const void* b) {
	if (*(const usize*)a > *(const usize*)b) return -1;
	if (*(const usize*)a < *(const usize*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* F32                                                                       */
/* ========================================================================= */

static inline int f32_sort_ascending(const void* a, const void* b) {
	if (*(const f32*)a < *(const f32*)b) return -1;
	if (*(const f32*)a > *(const f32*)b) return 1;
	return 0;
}

static inline int f32_sort_descending(const void* a, const void* b) {
	if (*(const f32*)a > *(const f32*)b) return -1;
	if (*(const f32*)a < *(const f32*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* F64                                                                       */
/* ========================================================================= */

static inline int f64_sort_ascending(const void* a, const void* b) {
	if (*(const f64*)a < *(const f64*)b) return -1;
	if (*(const f64*)a > *(const f64*)b) return 1;
	return 0;
}

static inline int f64_sort_descending(const void* a, const void* b) {
	if (*(const f64*)a > *(const f64*)b) return -1;
	if (*(const f64*)a < *(const f64*)b) return 1;
	return 0;
}

/* ========================================================================= */
/* REAL                                                                      */
/* ========================================================================= */

static inline int real_sort_ascending(const void* a, const void* b) {
	if (*(const real*)a < *(const real*)b) return -1;
	if (*(const real*)a > *(const real*)b) return 1;
	return 0;
}

static inline int real_sort_descending(const void* a, const void* b) {
	if (*(const real*)a > *(const real*)b) return -1;
	if (*(const real*)a < *(const real*)b) return 1;
	return 0;
}

#endif /* DEFINITIONS_SORT_H */