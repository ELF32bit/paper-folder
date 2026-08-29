/*  Written in 2026 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

/*

This is a minimal library to perform arithmetic in the ring F₂[x] / (charpoly),
where charpoly is the characteristic polynomial of the transition map of a
linear pseudorandom number generator.

The only purpose of this code is to make the arbitrary-jump code of our linear
generators self contained. Calling next() once is equivalent to multiplying the
state by x in this ring; thus, jumping ahead by n steps is equivalent to
multiplying by x^n mod charpoly, and the polynomial x^n mod charpoly can be
applied to the state with the same accumulate-and-step loop used by the
predefined jump() functions.

Exponentiation is performed by iterated squaring (square-and-multiply), so a
jump by an arbitrary distance n costs just O(log n) ring multiplications. Each
multiplication, however, uses bit-serial schoolbook multiplication and
reduction, which is far from optimal (a carryless-multiply instruction such as
PCLMULQDQ would be much faster); but jumping is a one-time, per-stream operation
whose cost is negligible against the stream that follows.

Before including this file, define POLY_DEG to the degree of the
characteristic polynomial (i.e., the number of bits of state) and provide a

	static const uint64_t charpoly[POLY_WORDS];

holding the POLY_DEG low-order coefficients of the (monic) characteristic
polynomial; coefficient k is bit (k & 63) of word (k >> 6). The leading
coefficient (the x^POLY_DEG term, which is 1) is implicit and not stored.

This code is intentionally not documented to discourage general usage.

If you would rather use an external library than this file, you can use Victor
Shoup's NTL library (https://libntl.org), in which polynomials over F₂ have type
GF2X, and the whole jump polynomial is a one-liner, PowerXMod():

    #include <NTL/GF2X.h>
    using namespace NTL;

    GF2X p;                                   // the characteristic polynomial
    for (long k = 0; k < POLY_DEG; k++)       // its low coefficients...
        if ((charpoly[k >> 6] >> (k & 63)) & 1) SetCoeff(p, k);
    SetCoeff(p, POLY_DEG);                    // ...and the implicit leading term
    GF2XModulus P(p);                         // a precomputed modulus

    ZZ n = conv<ZZ>(c) << e;                  // the jump distance c * 2^e
    GF2X g = PowerXMod(n, P);                 // g = x^n mod p

The coefficients of g are then applied to the state by the same accumulate-and-
step loop used in jump_n(). The same computation is available from FLINT
(https://flintlib.org), from a computer algebra system (in Sage, "R.<x> =
GF(2)[]" followed by "power_mod(x, n, p)"), or, at a lower level, from the
dedicated gf2x library (https://gitlab.inria.fr/gf2x/gf2x), which provides the
fast F₂[x] multiplication underlying such tools.

*/

#include <stdint.h>
#include <string.h>

#define POLY_WORDS (((POLY_DEG) + 63) / 64)

// a <- a * x mod charpoly
static void f2x_mulx(uint64_t *const a) {
	uint64_t carry = 0;
	for (int i = 0; i < POLY_WORDS; i++) {
		const uint64_t next_carry = a[i] >> 63;
		a[i] = (a[i] << 1) | carry;
		carry = next_carry;
	}
	// Coefficient of x^POLY_DEG after the shift.
	int top;
#if (POLY_DEG) % 64 == 0
	top = (int)carry;
#else
	top = (a[POLY_DEG >> 6] >> (POLY_DEG & 63)) & 1;
	a[POLY_DEG >> 6] &= ~(UINT64_C(1) << (POLY_DEG & 63));
#endif
	if (top)
		for (int i = 0; i < POLY_WORDS; i++)
			a[i] ^= charpoly[i];
}

// a <- a * b mod charpoly (Horner over the bits of a, from the top)
static void f2x_mulmod(uint64_t *const a, const uint64_t *const b) {
	uint64_t r[POLY_WORDS];
	memset(r, 0, sizeof r);
	for (int k = POLY_DEG - 1; k >= 0; k--) {
		f2x_mulx(r);
		if ((a[k >> 6] >> (k & 63)) & 1)
			for (int i = 0; i < POLY_WORDS; i++)
			  r[i] ^= b[i];
	}
	memcpy(a, r, sizeof r);
}

// out <- x^(c * 2^e) mod charpoly
static void f2x_jumppoly_ce(uint64_t c, uint32_t e, uint64_t *const out) {
	memset(out, 0, POLY_WORDS * sizeof *out);
	out[0] = 1;                     // out = 1
	for (int k = 63; k >= 0; k--) { // out = x^c
		f2x_mulmod(out, out);
		if ((c >> k) & 1)
			f2x_mulx(out);
	}
	while (e--)
		f2x_mulmod(out, out); // out = (x^c)^(2^e) = x^(c * 2^e)
}

// out <- x^n mod charpoly, where n = jump[0] + jump[1] * 2^64 + ... is the
// little-endian integer held in the len words of jump (square-and-multiply
// over the bits of n, from the most significant down).
static void f2x_jumppoly_n(const uint64_t *const jump, const int len, uint64_t *const out) {
	memset(out, 0, POLY_WORDS * sizeof *out);
	out[0] = 1;                                // out = 1
	for (int k = len * 64 - 1; k >= 0; k--) {  // out = x^n
		f2x_mulmod(out, out);
		if ((jump[k >> 6] >> (k & 63)) & 1)
			f2x_mulx(out);
	}
}
