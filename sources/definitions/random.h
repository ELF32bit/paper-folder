#ifndef DEFINITIONS_RANDOM_H
#define DEFINITIONS_RANDOM_H

#include "bool.h"
#include "integer.h"
#include "float.h"

typedef struct RNG {
	u64 state[4];
	u64 seed;
} RNG;

void random_set_state(RNG* rng, const u64 state[4]);
void random_get_state(const RNG* rng, u64 state[4]);

void random_set_seed(RNG* rng, u64 seed);
u64 random_get_seed(const RNG* rng);

/* ========================================================================= */
/* BOOL                                                                      */
/* ========================================================================= */

bool random_bool(RNG* rng);

bool random_bool_with_probability(RNG* rng, f32 probability);

/* ========================================================================= */
/* U8                                                                        */
/* ========================================================================= */

u8 random_u8(RNG* rng);

u8 random_u8_in_range(RNG* rng, u8 min, u8 max);

/* ========================================================================= */
/* U16                                                                       */
/* ========================================================================= */

u16 random_u16(RNG* rng);

u16 random_u16_in_range(RNG* rng, u16 min, u16 max);

/* ========================================================================= */
/* U32                                                                       */
/* ========================================================================= */

u32 random_u32(RNG* rng);

u32 random_u32_in_range(RNG* rng, u32 min, u32 max);

/* ========================================================================= */
/* U64                                                                       */
/* ========================================================================= */

u64 random_u64(RNG* rng);

u64 random_u64_in_range(RNG* rng, u64 min, u64 max);

/* ========================================================================= */
/* USIZE                                                                     */
/* ========================================================================= */

usize random_usize(RNG* rng);

usize random_usize_in_range(RNG* rng, usize min, usize max);

/* ========================================================================= */
/* F32                                                                       */
/* ========================================================================= */

f32 random_f32(RNG* rng);

f32 random_f32_in_range(RNG* rng, f32 min, f32 max);

/* ========================================================================= */
/* F64                                                                       */
/* ========================================================================= */

f64 random_f64(RNG* rng);

f64 random_f64_in_range(RNG* rng, f64 min, f64 max);

/* ========================================================================= */
/* REAL                                                                      */
/* ========================================================================= */

real random_real(RNG* rng);

real random_real_in_range(RNG* rng, real min, real max);

/* ========================================================================= */
/* I8                                                                        */
/* ========================================================================= */

static inline i8 random_i8(RNG* rng) {
	return (i8)random_u8(rng);
}

static inline i8 random_i8_in_range(RNG* rng, i8 min, i8 max) {
	return min + (i8)random_u8_in_range(rng, 0, (u8)(max - min));
}

/* ========================================================================= */
/* I16                                                                       */
/* ========================================================================= */

static inline i16 random_i16(RNG* rng) {
	return (i16)random_u16(rng);
}

static inline i16 random_i16_in_range(RNG* rng, i16 min, i16 max) {
	return min + (i16)random_u16_in_range(rng, 0, (u16)(max - min));
}

/* ========================================================================= */
/* I32                                                                       */
/* ========================================================================= */

static inline i32 random_i32(RNG* rng) {
	return (i32)random_u32(rng);
}

static inline i32 random_i32_in_range(RNG* rng, i32 min, i32 max) {
	return min + (i32)random_u32_in_range(rng, 0, (u32)(max - min));
}

/* ========================================================================= */
/* I64                                                                       */
/* ========================================================================= */

static inline i64 random_i64(RNG* rng) {
	return (i64)random_u64(rng);
}

static inline i64 random_i64_in_range(RNG* rng, i64 min, i64 max) {
	return min + (i64)random_u64_in_range(rng, 0, (u64)(max - min));
}

/* ========================================================================= */
/* ISIZE                                                                       */
/* ========================================================================= */

static inline isize random_isize(RNG* rng) {
	return (isize)random_usize(rng);
}

static inline isize random_isize_in_range(RNG* rng, isize min, isize max) {
	return min + (isize)random_usize_in_range(rng, 0, (usize)(max - min));
}

#endif /* DEFINITIONS_RANDOM_H */