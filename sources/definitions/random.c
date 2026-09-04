#include "random.h"

#include "xoshiro256starstar.c"

#include "cpu.h"

static u64 global_seed = 0;

#define LOCAL_CONTEXT_START(rng) \
	u64 global_state[4]; \
	if (rng != NULL) { \
		global_state[0] = s[0]; \
		global_state[1] = s[1]; \
		global_state[2] = s[2]; \
		global_state[3] = s[3]; \
		s[0] = rng->state[0]; \
		s[1] = rng->state[1]; \
		s[2] = rng->state[2]; \
		s[3] = rng->state[3]; \
	}

#define LOCAL_CONTEXT_END(rng) \
	if (rng != NULL) { \
		rng->state[0] = s[0]; \
		rng->state[1] = s[1]; \
		rng->state[2] = s[2]; \
		rng->state[3] = s[3]; \
		s[0] = global_state[0]; \
		s[1] = global_state[1]; \
		s[2] = global_state[2]; \
		s[3] = global_state[3]; \
	}

void random_set_state(RNG* rng, const u64 state[4]) {
	if (rng != NULL) {
		rng->state[0] = state[0];
		rng->state[1] = state[1];
		rng->state[2] = state[2];
		rng->state[3] = state[3];
	} else {
		s[0] = state[0];
		s[1] = state[1];
		s[2] = state[2];
		s[3] = state[3];
	}
}

void random_get_state(const RNG* rng, u64 state[4]) {
	if (rng != NULL) {
		state[0] = rng->state[0];
		state[1] = rng->state[1];
		state[2] = rng->state[2];
		state[3] = rng->state[3];
	} else {
		state[0] = s[0];
		state[1] = s[1];
		state[2] = s[2];
		state[3] = s[3];
	}
}

static inline u64 _xoshiro_splitmix64_next(u64* state) {
	u64 z = (*state += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

void random_set_seed(RNG* rng, u64 seed) {
	u64 S = seed;
	u64 s0 = _xoshiro_splitmix64_next(&S);
	u64 s1 = _xoshiro_splitmix64_next(&S);
	u64 s2 = _xoshiro_splitmix64_next(&S);
	u64 s3 = _xoshiro_splitmix64_next(&S);
	if (rng != NULL) {
		rng->seed = seed;
		rng->state[0] = s0;
		rng->state[1] = s1;
		rng->state[2] = s2;
		rng->state[3] = s3;
	} else {
		global_seed = seed;
		s[0] = s0;
		s[1] = s1;
		s[2] = s2;
		s[3] = s3;
	}
}

u64 random_get_seed(const RNG* rng) {
	return (rng != NULL)
		? rng->seed
		: global_seed;
}

/* ========================================================================= */
/* BOOL                                                                      */
/* ========================================================================= */

bool random_bool(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	bool result = BOOL(next() & 1);
	LOCAL_CONTEXT_END(rng);
	return result;
}

bool random_bool_with_probability(RNG* rng, f32 probability) {
	return
		(probability <= 0.0f) ? false
		: (probability >= 1.0f) ? true
		: random_f32(rng) < probability;
}

/* ========================================================================= */
/* U8                                                                        */
/* ========================================================================= */

u8 random_u8(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	u8 result = (u8)next();
	LOCAL_CONTEXT_END(rng);
	return result;
}

u8 random_u8_in_range(RNG* rng, u8 min, u8 max) {
	u8 range = max - min;
	if (range == U8_MAX) {
		return random_u8(rng);
	}
	range += 1;
	u8 limit = U8_MAX - (U8_MAX % range);
	u8 r;
	do {
		r = random_u8(rng);
	} while (r >= limit);
	return min + (r % range);
}

/* ========================================================================= */
/* U16                                                                       */
/* ========================================================================= */

u16 random_u16(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	u16 result = (u16)next();
	LOCAL_CONTEXT_END(rng);
	return result;
}

u16 random_u16_in_range(RNG* rng, u16 min, u16 max) {
	u16 range = max - min;
	if (range == U16_MAX) {
		return random_u16(rng);
	}
	range += 1;
	u16 limit = U16_MAX - (U16_MAX % range);
	u16 r;
	do {
		r = random_u16(rng);
	} while (r >= limit);
	return min + (r % range);
}

/* ========================================================================= */
/* U32                                                                       */
/* ========================================================================= */

u32 random_u32(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	u32 result = (u32)next();
	LOCAL_CONTEXT_END(rng);
	return result;
}

u32 random_u32_in_range(RNG* rng, u32 min, u32 max) {
	u32 range = max - min;
	if (range == U32_MAX) {
		return random_u32(rng);
	}
	range += 1;
	u32 limit = U32_MAX - (U32_MAX % range);
	u32 r;
	do {
		r = random_u32(rng);
	} while (r >= limit);
	return min + (r % range);
}

/* ========================================================================= */
/* U64                                                                       */
/* ========================================================================= */

u64 random_u64(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	u64 result = (u64)next();
	LOCAL_CONTEXT_END(rng);
	return result;
}

u64 random_u64_in_range(RNG* rng, u64 min, u64 max) {
	u64 range = max - min;
	if (range == U64_MAX) {
		return random_u64(rng);
	}
	range += 1;
	u64 limit = U64_MAX - (U64_MAX % range);
	u64 r;
	do {
		r = random_u64(rng);
	} while (r >= limit);
	return min + (r % range);
}

/* ========================================================================= */
/* USIZE                                                                     */
/* ========================================================================= */

usize random_usize(RNG* rng) {
#if defined(CPU_ARCHITECTURE_AT_LEAST_64_BIT)
	return (usize)random_u64(rng);
#elif defined(CPU_ARCHITECTURE_AT_LEAST_32_BIT)
	return (usize)random_u32(rng);
#elif defined(CPU_ARCHITECTURE_AT_LEAST_16_BIT)
	return (usize)random_u16(rng);
#else
	return (usize)random_u8(rng);
#endif
}

usize random_usize_in_range(RNG* rng, usize min, usize max) {
	usize range = max - min;
	if (range == USIZE_MAX) {
		return random_usize(rng);
	}
	range += 1;
	usize limit = USIZE_MAX - (USIZE_MAX % range);
	usize r;
	do {
		r = random_usize(rng);
	} while (r >= limit);
	return min + (r % range);
}

/* ========================================================================= */
/* F32                                                                       */
/* ========================================================================= */

f32 random_f32(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	f32 result = (f32)((next() >> 40) * 0x1.0p-24f);
	LOCAL_CONTEXT_END(rng);
	return result;
}

f32 random_f32_in_range(RNG* rng, f32 min, f32 max) {
	return min + (random_f32(rng) * (max - min));
}

/* ========================================================================= */
/* F64                                                                       */
/* ========================================================================= */

f64 random_f64(RNG* rng) {
	LOCAL_CONTEXT_START(rng);
	f64 result = (f64)((next() >> 11) * 0x1.0p-53);
	LOCAL_CONTEXT_END(rng);
	return result;
}

f64 random_f64_in_range(RNG* rng, f64 min, f64 max) {
	return min + (random_f64(rng) * (max - min));
}

/* ========================================================================= */
/* REAL                                                                      */
/* ========================================================================= */

real random_real(RNG* rng) {
	if (sizeof(real) == sizeof(f32)) {
		return (real)random_f32(rng);
	} else if (sizeof(real) == sizeof(f64)) {
		return (real)random_f64(rng);
	} else if (sizeof(real) == sizeof(long double)) {
		return (real)random_f64(rng);
	} else {
		return 0.0;
	}
}

real random_real_in_range(RNG* rng, real min, real max) {
	return min + (random_real(rng) * (max - min));
}