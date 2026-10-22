#include "hash.h"

#include "cpu.h"
#include <string.h>

/* ========================================================================= */
/* U8                                                                        */
/* ========================================================================= */

#define U8_MIX_PRIME 0x9DU

u8 hash_u8(u8 a) {
	u8 h = a;
	h = (h ^ 61U) ^ (h >> 4);
	h = h + (h << 3);
	h = h ^ (h >> 3);
	h = h * 0x9DU;
	h = h ^ (h >> 4);
	return h;
}

u8 hash_u8_pack2(u8 a, u8 b) {
	return hash_u8(
		(a & 0x0FU) |
		((b & 0x0FU) << 4));
}

u8 hash_u8_mix2(u8 a, u8 b) {
	u8 h = hash_u8(a);
	h = (h ^ b) * U8_MIX_PRIME;
	return hash_u8(h);
}

u8 hash_u8_pack3(u8 a, u8 b, u8 c) {
	return hash_u8(
		(a & 0x07U) |
		((b & 0x07U) << 3) |
		((c & 0x03U) << 6));
}

u8 hash_u8_mix3(u8 a, u8 b, u8 c) {
	u8 h = hash_u8(a);
	h = (h ^ b) * U8_MIX_PRIME;
	h = (h ^ c) * U8_MIX_PRIME;
	return hash_u8(h);
}

u8 hash_u8_pack4(u8 a, u8 b, u8 c, u8 d) {
	return hash_u8(
		(a & 0x03U) |
		((b & 0x03U) << 2) |
		((c & 0x03U) << 4) |
		((d & 0x03U) << 6));
}

u8 hash_u8_mix4(u8 a, u8 b, u8 c, u8 d) {
	u8 h = hash_u8(a);
	h = (h ^ b) * U8_MIX_PRIME;
	h = (h ^ c) * U8_MIX_PRIME;
	h = (h ^ d) * U8_MIX_PRIME;
	return hash_u8(h);
}

/* ========================================================================= */
/* U16                                                                       */
/* ========================================================================= */

#define U16_MIX_PRIME 0x9E37U

u16 hash_u16(u16 a) {
	u16 h = a;
	h = (h ^ 61U) ^ (h >> 8);
	h = h + (h << 3);
	h = h ^ (h >> 4);
	h = h * 0x9E37U;
	h = h ^ (h >> 8);
	return h;
}

u16 hash_u16_pack2(u16 a, u16 b) {
	return hash_u16(
		(a & 0xFFU) |
		((b & 0xFFU) << 8));
}

u16 hash_u16_mix2(u16 a, u16 b) {
	u16 h = hash_u16(a);
	h = (h ^ b) * U16_MIX_PRIME;
	return hash_u16(h);
}

u16 hash_u16_pack3(u16 a, u16 b, u16 c) {
	return hash_u16(
		(a & 0x1FU) |
		((b & 0x1FU) << 5) |
		((c & 0x1FU) << 10));
}

u16 hash_u16_mix3(u16 a, u16 b, u16 c) {
	u16 h = hash_u16(a);
	h = (h ^ b) * U16_MIX_PRIME;
	h = (h ^ c) * U16_MIX_PRIME;
	return hash_u16(h);
}

u16 hash_u16_pack4(u16 a, u16 b, u16 c, u16 d) {
	return hash_u16(
		(a & 0x0FU) |
		((b & 0x0FU) << 4) |
		((c & 0x0FU) << 8) |
		((d & 0x0FU) << 12));
}

u16 hash_u16_mix4(u16 a, u16 b, u16 c, u16 d) {
	u16 h = hash_u16(a);
	h = (h ^ b) * U16_MIX_PRIME;
	h = (h ^ c) * U16_MIX_PRIME;
	h = (h ^ d) * U16_MIX_PRIME;
	return hash_u16(h);
}

/* ========================================================================= */
/* U32                                                                       */
/* ========================================================================= */

#define U32_MIX_PRIME 0x9e3779b9U

u32 hash_u32(u32 a) {
	u32 h = a;
	h = (h ^ 61U) ^ (h >> 16);
	h = h + (h << 3);
	h = h ^ (h >> 4);
	h = h * 0x27d4eb2dU;
	h = h ^ (h >> 15);
	return h;
}

u32 hash_u32_pack2(u32 a, u32 b) {
	return hash_u32(
		(a & 0xFFFFU) |
		((b & 0xFFFFU) << 16));
}

u32 hash_u32_mix2(u32 a, u32 b) {
	u32 h = hash_u32(a);
	h = (h ^ b) * U32_MIX_PRIME;
	return hash_u32(h);
}

u32 hash_u32_pack3(u32 a, u32 b, u32 c) {
	return hash_u32(
		(a & 0x3FFU) |
		((b & 0x3FFU) << 10) |
		((c & 0x3FFU) << 20));
}

u32 hash_u32_mix3(u32 a, u32 b, u32 c) {
	u32 h = hash_u32(a);
	h = (h ^ b) * U32_MIX_PRIME;
	h = (h ^ c) * U32_MIX_PRIME;
	return hash_u32(h);
}

u32 hash_u32_pack4(u32 a, u32 b, u32 c, u32 d) {
	return hash_u32(
		(a & 0xFFU) |
		((b & 0xFFU) << 8) |
		((c & 0xFFU) << 16) |
		((d & 0xFFU) << 24));
}

u32 hash_u32_mix4(u32 a, u32 b, u32 c, u32 d) {
	u32 h = hash_u32(a);
	h = (h ^ b) * U32_MIX_PRIME;
	h = (h ^ c) * U32_MIX_PRIME;
	h = (h ^ d) * U32_MIX_PRIME;
	return hash_u32(h);
}

/* ========================================================================= */
/* U64                                                                       */
/* ========================================================================= */

#define U64_MIX_PRIME 0x9e3779b97f4a7c15ULL

u64 hash_u64(u64 a) {
	u64 h = a;
	h = (~h) + (h << 21);
	h = h ^ (h >> 24);
	h = (h + (h << 3)) + (h << 8);
	h = h ^ (h >> 14);
	h = (h + (h << 4)) + (h << 13);
	h = h ^ (h >> 28);
	h = h + (h << 31);
	return h;
}

u64 hash_u64_pack2(u64 a, u64 b) {
	return hash_u64(
		(a & 0xFFFFFFFFULL) |
		((b & 0xFFFFFFFFULL) << 32));
}

u64 hash_u64_mix2(u64 a, u64 b) {
	u64 h = hash_u64(a);
	h = (h ^ b) * U64_MIX_PRIME;
	return hash_u64(h);
}

u64 hash_u64_pack3(u64 a, u64 b, u64 c) {
	return hash_u64(
		(a & 0x1FFFFFULL) |
		((b & 0x1FFFFFULL) << 21) |
		((c & 0x1FFFFFULL) << 42));
}

u64 hash_u64_mix3(u64 a, u64 b, u64 c) {
	u64 h = hash_u64(a);
	h = (h ^ b) * U64_MIX_PRIME;
	h = (h ^ c) * U64_MIX_PRIME;
	return hash_u64(h);
}

u64 hash_u64_pack4(u64 a, u64 b, u64 c, u64 d) {
	return hash_u64(
		(a & 0xFFFFULL) |
		((b & 0xFFFFULL) << 16) |
		((c & 0xFFFFULL) << 32) |
		((d & 0xFFFFULL) << 48));
}

u64 hash_u64_mix4(u64 a, u64 b, u64 c, u64 d) {
	u64 h = hash_u64(a);
	h = (h ^ b) * U64_MIX_PRIME;
	h = (h ^ c) * U64_MIX_PRIME;
	h = (h ^ d) * U64_MIX_PRIME;
	return hash_u64(h);
}

/* ========================================================================= */
/* USIZE                                                                     */
/* ========================================================================= */

usize hash_usize(usize a) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64(U64(a)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32(U32(a)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16(U16(a)));
#else
	return USIZE(hash_u8(U8(a)));
#endif
}

usize hash_usize_pack2(usize a, usize b) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64_pack2(U64(a), U64(b)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32_pack2(U32(a), U32(b)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16_pack2(U16(a), U16(b)));
#else
	return USIZE(hash_u8_pack2(U8(a), U8(b)));
#endif
}

usize hash_usize_mix2(usize a, usize b) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64_mix2(U64(a), U64(b)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32_mix2(U32(a), U32(b)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16_mix2(U16(a), U16(b)));
#else
	return USIZE(hash_u8_mix2(U8(a), U8(b)));
#endif
}

usize hash_usize_pack3(usize a, usize b, usize c) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64_pack3(U64(a), U64(b), U64(c)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32_pack3(U32(a), U32(b), U32(c)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16_pack3(U16(a), U16(b), U16(c)));
#else
	return USIZE(hash_u8_pack3(U8(a), U8(b), U8(c)));
#endif
}

usize hash_usize_mix3(usize a, usize b, usize c) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64_mix3(U64(a), U64(b), U64(c)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32_mix3(U32(a), U32(b), U32(c)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16_mix3(U16(a), U16(b), U16(c)));
#else
	return USIZE(hash_u8_mix3(U8(a), U8(b), U8(c)));
#endif
}

usize hash_usize_pack4(usize a, usize b, usize c, usize d) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64_pack4(U64(a), U64(b), U64(c), U64(d)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32_pack4(U32(a), U32(b), U32(c), U32(d)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16_pack4(U16(a), U16(b), U16(c), U16(d)));
#else
	return USIZE(hash_u8_pack4(U8(a), U8(b), U8(c), U8(d)));
#endif
}

usize hash_usize_mix4(usize a, usize b, usize c, usize d) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return USIZE(hash_u64_mix4(U64(a), U64(b), U64(c), U64(d)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return USIZE(hash_u32_mix4(U32(a), U32(b), U32(c), U32(d)));
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return USIZE(hash_u16_mix4(U16(a), U16(b), U16(c), U16(d)));
#else
	return USIZE(hash_u8_mix4(U8(a), U8(b), U8(c), U8(d)));
#endif
}

/* ========================================================================= */
/* F32                                                                       */
/* ========================================================================= */

static inline u32 _f32_normalize(f32 number) {
	u32 bytes;
	memcpy(&bytes, &number, sizeof(f32));
	u32 is_zero = !(bytes & 0x7FFFFFFF);
	bytes &= ~(U32(is_zero) << 31);
	return bytes;
}

u32 hash_f32(f32 a) {
	return hash_u32(
		_f32_normalize(a));
}

u32 hash_f32_mix2(f32 a, f32 b) {
	return hash_u32_mix2(
		_f32_normalize(a),
		_f32_normalize(b));
}

u32 hash_f32_mix3(f32 a, f32 b, f32 c) {
	return hash_u32_mix3(
		_f32_normalize(a),
		_f32_normalize(b),
		_f32_normalize(c));
}

u32 hash_f32_mix4(f32 a, f32 b, f32 c, f32 d) {
	return hash_u32_mix4(
		_f32_normalize(a),
		_f32_normalize(b),
		_f32_normalize(c),
		_f32_normalize(d));
}

/* ========================================================================= */
/* F64                                                                       */
/* ========================================================================= */

static inline u64 _f64_normalize(f64 number) {
	u64 bytes;
	memcpy(&bytes, &number, sizeof(f64));
	u64 is_zero = !(bytes & 0x7FFFFFFFFFFFFFFFULL);
	bytes &= ~(U64(is_zero) << 63);
	return bytes;
}

u64 hash_f64(f64 a) {
	return hash_u64(
		_f64_normalize(a));
}

u64 hash_f64_mix2(f64 a, f64 b) {
	return hash_u64_mix2(
		_f64_normalize(a),
		_f64_normalize(b));
}

u64 hash_f64_mix3(f64 a, f64 b, f64 c) {
	return hash_u64_mix3(
		_f64_normalize(a),
		_f64_normalize(b),
		_f64_normalize(c));
}

u64 hash_f64_mix4(f64 a, f64 b, f64 c, f64 d) {
	return hash_u64_mix4(
		_f64_normalize(a),
		_f64_normalize(b),
		_f64_normalize(c),
		_f64_normalize(d));
}

/* ========================================================================= */
/* REAL                                                                      */
/* ========================================================================= */

usize hash_real(real a) {
	if (sizeof(real) == sizeof(f32)) {
		return USIZE(hash_f32(F32(a)));
	} else if (sizeof(real) == sizeof(f64)) {
		return USIZE(hash_f64(F64(a)));
	} else if (sizeof(real) == sizeof(long double)) {
		return USIZE(hash_f64(F64(a)));
	} else return 0;
}

usize hash_real_mix2(real a, real b) {
	if (sizeof(real) == sizeof(f32)) {
		return USIZE(hash_f32_mix2(F32(a), F32(b)));
	} else if (sizeof(real) == sizeof(f64)) {
		return USIZE(hash_f64_mix2(F64(a), F64(b)));
	} else if (sizeof(real) == sizeof(long double)) {
		return USIZE(hash_f64_mix2(F64(a), F64(b)));
	} else return 0;
}

usize hash_real_mix3(real a, real b, real c) {
	if (sizeof(real) == sizeof(f32)) {
		return USIZE(hash_f32_mix3(F32(a), F32(b), F32(c)));
	} else if (sizeof(real) == sizeof(f64)) {
		return USIZE(hash_f64_mix3(F64(a), F64(b), F64(c)));
	} else if (sizeof(real) == sizeof(long double)) {
		return USIZE(hash_f64_mix3(F64(a), F64(b), F64(c)));
	} else return 0;
}

usize hash_real_mix4(real a, real b, real c, real d) {
	if (sizeof(real) == sizeof(f32)) {
		return USIZE(hash_f32_mix4(F32(a), F32(b), F32(c), F32(d)));
	} else if (sizeof(real) == sizeof(f64)) {
		return USIZE(hash_f64_mix4(F64(a), F64(b), F64(c), F64(d)));
	} else if (sizeof(real) == sizeof(long double)) {
		return USIZE(hash_f64_mix4(F64(a), F64(b), F64(c), F64(d)));
	} else return 0;
}