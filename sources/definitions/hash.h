#ifndef DEFINITIONS_HASH_H
#define DEFINITIONS_HASH_H

#include "integer.h"
#include "float.h"

/* ========================================================================= */
/* U8 Section                                                                */
/* ========================================================================= */

u8 hash_u8(u8 a);

/* Collision-free when (a, b) <= 15 */
u8 hash_u8_pack2(u8 a, u8 b);

u8 hash_u8_mix2(u8 a, u8 b);

/* Collision-free when (a, b <= 7, c <= 3) */
u8 hash_u8_pack3(u8 a, u8 b, u8 c);

u8 hash_u8_mix3(u8 a, u8 b, u8 c);

/* Collision-free when (a, b, c, d) <= 3 */
u8 hash_u8_pack4(u8 a, u8 b, u8 c, u8 d);

u8 hash_u8_mix4(u8 a, u8 b, u8 c, u8 d);

/* ========================================================================= */
/* U16 Section                                                               */
/* ========================================================================= */

u16 hash_u16(u16 a);

/* Collision-free when (a, b) <= 255 */
u16 hash_u16_pack2(u16 a, u16 b);

u16 hash_u16_mix2(u16 a, u16 b);

/* Collision-free when (a, b, c) <= 31 */
u16 hash_u16_pack3(u16 a, u16 b, u16 c);

u16 hash_u16_mix3(u16 a, u16 b, u16 c);

/* Collision-free when (a, b, c, d) <= 15 */
u16 hash_u16_pack4(u16 a, u16 b, u16 c, u16 d);

u16 hash_u16_mix4(u16 a, u16 b, u16 c, u16 d);

/* ========================================================================= */
/* U32 Section                                                               */
/* ========================================================================= */

u32 hash_u32(u32 a);

/* Collision-free when (a, b) <= 65535 */
u32 hash_u32_pack2(u32 a, u32 b);

u32 hash_u32_mix2(u32 a, u32 b);

/* Collision-free when (a, b, c) <= 1023 */
u32 hash_u32_pack3(u32 a, u32 b, u32 c);

u32 hash_u32_mix3(u32 a, u32 b, u32 c);

/* Collision-free when (a, b, c, d) <= 255 */
u32 hash_u32_pack4(u32 a, u32 b, u32 c, u32 d);

u32 hash_u32_mix4(u32 a, u32 b, u32 c, u32 d);

/* ========================================================================= */
/* U64 Section                                                               */
/* ========================================================================= */

u64 hash_u64(u64 a);

/* Collision-free when (a, b) <= 4294967295 */
u64 hash_u64_pack2(u64 a, u64 b);

u64 hash_u64_mix2(u64 a, u64 b);

/* Collision-free when (a, b, c) <= 2097151 */
u64 hash_u64_pack3(u64 a, u64 b, u64 c);

u64 hash_u64_mix3(u64 a, u64 b, u64 c);

/* Collision-free when (a, b, c, d) <= 65535 */
u64 hash_u64_pack4(u64 a, u64 b, u64 c, u64 d);

u64 hash_u64_mix4(u64 a, u64 b, u64 c, u64 d);

/* ========================================================================= */
/* USIZE Section                                                             */
/* ========================================================================= */

usize hash_usize(usize a);

usize hash_usize_pack2(usize a, usize b);

usize hash_usize_mix2(usize a, usize b);

usize hash_usize_pack3(usize a, usize b, usize c);

usize hash_usize_mix3(usize a, usize b, usize c);

usize hash_usize_pack4(usize a, usize b, usize c, usize d);

usize hash_usize_mix4(usize a, usize b, usize c, usize d);

/* ========================================================================= */
/* F32 Section                                                               */
/* ========================================================================= */

u32 hash_f32(f32 a);

u32 hash_f32_mix2(f32 a, f32 b);

u32 hash_f32_mix3(f32 a, f32 b, f32 c);

u32 hash_f32_mix4(f32 a, f32 b, f32 c, f32 d);

/* ========================================================================= */
/* F64 Section                                                               */
/* ========================================================================= */

u64 hash_f64(f64 a);

u64 hash_f64_mix2(f64 a, f64 b);

u64 hash_f64_mix3(f64 a, f64 b, f64 c);

u64 hash_f64_mix4(f64 a, f64 b, f64 c, f64 d);

/* ========================================================================= */
/* REAL Section                                                              */
/* ========================================================================= */

usize hash_real(real a);

usize hash_real_mix2(real a, real b);

usize hash_real_mix3(real a, real b, real c);

usize hash_real_mix4(real a, real b, real c, real d);

/* ========================================================================= */
/* I8 Section                                                                */
/* ========================================================================= */

static inline i8 hash_i8(i8 a) {
	return (i8)hash_u8((u8)a);
}

static inline i8 hash_i8_pack2(i8 a, i8 b) {
	return (i8)hash_u8_pack2((u8)a, (u8)b);
}

static inline i8 hash_i8_mix2(i8 a, i8 b) {
	return (i8)hash_u8_mix2((u8)a, (u8)b);
}

static inline i8 hash_i8_pack3(i8 a, i8 b, i8 c) {
	return (i8)hash_u8_pack3((u8)a, (u8)b, (u8)c);
}

static inline i8 hash_i8_mix3(i8 a, i8 b, i8 c) {
	return (i8)hash_u8_mix3((u8)a, (u8)b, (u8)c);
}

static inline i8 hash_i8_pack4(i8 a, i8 b, i8 c, i8 d) {
	return (i8)hash_u8_pack4((u8)a, (u8)b, (u8)c, (u8)d);
}

static inline i8 hash_i8_mix4(i8 a, i8 b, i8 c, i8 d) {
	return (i8)hash_u8_mix4((u8)a, (u8)b, (u8)c, (u8)d);
}

/* ========================================================================= */
/* I16 Section                                                               */
/* ========================================================================= */

static inline i16 hash_i16(i16 a) {
	return (i16)hash_u16((u16)a);
}

static inline i16 hash_i16_pack2(i16 a, i16 b) {
	return (i16)hash_u16_pack2((u16)a, (u16)b);
}

static inline i16 hash_i16_mix2(i16 a, i16 b) {
	return (i16)hash_u16_mix2((u16)a, (u16)b);
}

static inline i16 hash_i16_pack3(i16 a, i16 b, i16 c) {
	return (i16)hash_u16_pack3((u16)a, (u16)b, (u16)c);
}

static inline i16 hash_i16_mix3(i16 a, i16 b, i16 c) {
	return (i16)hash_u16_mix3((u16)a, (u16)b, (u16)c);
}

static inline i16 hash_i16_pack4(i16 a, i16 b, i16 c, i16 d) {
	return (i16)hash_u16_pack4((u16)a, (u16)b, (u16)c, (u16)d);
}

static inline i16 hash_i16_mix4(i16 a, i16 b, i16 c, i16 d) {
	return (i16)hash_u16_mix4((u16)a, (u16)b, (u16)c, (u16)d);
}

/* ========================================================================= */
/* I32 Section                                                               */
/* ========================================================================= */

static inline i32 hash_i32(i32 a) {
	return (i32)hash_u32((u32)a);
}

static inline i32 hash_i32_pack2(i32 a, i32 b) {
	return (i32)hash_u32_pack2((u32)a, (u32)b);
}

static inline i32 hash_i32_mix2(i32 a, i32 b) {
	return (i32)hash_u32_mix2((u32)a, (u32)b);
}

static inline i32 hash_i32_pack3(i32 a, i32 b, i32 c) {
	return (i32)hash_u32_pack3((u32)a, (u32)b, (u32)c);
}

static inline i32 hash_i32_mix3(i32 a, i32 b, i32 c) {
	return (i32)hash_u32_mix3((u32)a, (u32)b, (u32)c);
}

static inline i32 hash_i32_pack4(i32 a, i32 b, i32 c, i32 d) {
	return (i32)hash_u32_pack4((u32)a, (u32)b, (u32)c, (u32)d);
}

static inline i32 hash_i32_mix4(i32 a, i32 b, i32 c, i32 d) {
	return (i32)hash_u32_mix4((u32)a, (u32)b, (u32)c, (u32)d);
}

/* ========================================================================= */
/* I64 Section                                                               */
/* ========================================================================= */

static inline i64 hash_i64(i64 a) {
	return (i64)hash_u64((u64)a);
}

static inline i64 hash_i64_pack2(i64 a, i64 b) {
	return (i64)hash_u64_pack2((u64)a, (u64)b);
}

static inline i64 hash_i64_mix2(i64 a, i64 b) {
	return (i64)hash_u64_mix2((u64)a, (u64)b);
}

static inline i64 hash_i64_pack3(i64 a, i64 b, i64 c) {
	return (i64)hash_u64_pack3((u64)a, (u64)b, (u64)c);
}

static inline i64 hash_i64_mix3(i64 a, i64 b, i64 c) {
	return (i64)hash_u64_mix3((u64)a, (u64)b, (u64)c);
}

static inline i64 hash_i64_pack4(i64 a, i64 b, i64 c, i64 d) {
	return (i64)hash_u64_pack4((u64)a, (u64)b, (u64)c, (u64)d);
}

static inline i64 hash_i64_mix4(i64 a, i64 b, i64 c, i64 d) {
	return (i64)hash_u64_mix4((u64)a, (u64)b, (u64)c, (u64)d);
}

/* ========================================================================= */
/* ISIZE Section                                                             */
/* ========================================================================= */

static inline isize hash_isize(isize a) {
	return (isize)hash_usize((usize)a);
}

static inline isize hash_isize_pack2(isize a, isize b) {
	return (isize)hash_usize_pack2((usize)a, (usize)b);
}

static inline isize hash_isize_mix2(isize a, isize b) {
	return (isize)hash_usize_mix2((usize)a, (usize)b);
}

static inline isize hash_isize_pack3(isize a, isize b, isize c) {
	return (isize)hash_usize_pack3((usize)a, (usize)b, (usize)c);
}

static inline isize hash_isize_mix3(isize a, isize b, isize c) {
	return (isize)hash_usize_mix3((usize)a, (usize)b, (usize)c);
}

static inline isize hash_isize_pack4(isize a, isize b, isize c, isize d) {
	return (isize)hash_usize_pack4((usize)a, (usize)b, (usize)c, (usize)d);
}

static inline isize hash_isize_mix4(isize a, isize b, isize c, isize d) {
	return (isize)hash_usize_mix4((usize)a, (usize)b, (usize)c, (usize)d);
}

/* ========================================================================= */
/* Template Section                                                          */
/* ========================================================================= */

static inline u8 u8_hash(const void* a) {
	return (u8)hash_u8(*(const u8*)a);
}

static inline u8 u8_hash_identity(const void* a) {
	return *(const u8*)a;
}

static inline u16 u16_hash(const void* a) {
	return (u16)hash_u16(*(const u16*)a);
}

static inline u16 u16_hash_identity(const void* a) {
	return *(const u16*)a;
}

static inline u32 u32_hash(const void* a) {
	return (u32)hash_u32(*(const u32*)a);
}

static inline u32 u32_hash_identity(const void* a) {
	return *(const u32*)a;
}

static inline u64 u64_hash(const void* a) {
	return (u64)hash_u64(*(const u64*)a);
}

static inline u64 u64_hash_identity(const void* a) {
	return *(const u64*)a;
}

static inline usize usize_hash(const void* a) {
	return (usize)hash_usize(*(const usize*)a);
}

static inline usize usize_hash_identity(const void* a) {
	return *(const usize*)a;
}

static inline i8 i8_hash(const void* a) {
	return (i8)hash_i8(*(const i8*)a);
}

static inline i8 i8_hash_identity(const void* a) {
	return *(const i8*)a;
}

static inline i16 i16_hash(const void* a) {
	return (i16)hash_i16(*(const i16*)a);
}

static inline i16 i16_hash_identity(const void* a) {
	return *(const i16*)a;
}

static inline i32 i32_hash(const void* a) {
	return (i32)hash_i32(*(const i32*)a);
}

static inline i32 i32_hash_identity(const void* a) {
	return *(const i32*)a;
}

static inline i64 i64_hash(const void* a) {
	return (i64)hash_i64(*(const i64*)a);
}

static inline i64 i64_hash_identity(const void* a) {
	return *(const i64*)a;
}

static inline isize isize_hash(const void* a) {
	return (isize)hash_isize(*(const isize*)a);
}

static inline isize isize_hash_identity(const void* a) {
	return *(const isize*)a;
}

static inline f32 f32_hash(const void* a) {
	return (f32)hash_f32(*(const f32*)a);
}

static inline f32 f32_hash_identity(const void* a) {
	return *(const f32*)a;
}

static inline f64 f64_hash(const void* a) {
	return (f64)hash_f64(*(const f64*)a);
}

static inline f64 f64_hash_identity(const void* a) {
	return *(const f64*)a;
}

static inline real real_hash(const void* a) {
	return (real)hash_real(*(const real*)a);
}

static inline real real_hash_identity(const void* a) {
	return *(const real*)a;
}

#endif /* DEFINITIONS_HASH_H */