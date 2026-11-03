#ifndef MATH_COLOR_H
#define MATH_COLOR_H

#include "definitions/types.h"
#include "definitions/math.h"
#include "definitions/hash.h"

#include <stdio.h>
#include <string.h>

#define COLOR_EPSILON 1e-3

typedef struct Color {
	union {
		struct { f32 r, g, b, a; };
		f32 components[4];
	};
} Color;

/* ========================================================================= */
/* Creation                                                                  */
/* ========================================================================= */

static inline Color color_zero(void) {
	Color c;
	c.r = 0.0;
	c.g = 0.0;
	c.b = 0.0;
	c.a = 0.0;
	return c;
}

static inline Color color_black(void) {
	Color c;
	c.r = 0.0;
	c.g = 0.0;
	c.b = 0.0;
	c.a = 1.0;
	return c;
}

static inline Color color_white(void) {
	Color c;
	c.r = 1.0;
	c.g = 1.0;
	c.b = 1.0;
	c.a = 1.0;
	return c;
}

static inline Color color_from_components(f32 r, f32 g, f32 b, f32 a) {
	Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

static inline Color color_from_rgba8(u8 r, u8 g, u8 b, u8 a) {
	Color c;
	c.r = r / 255.0;
	c.g = g / 255.0;
	c.b = b / 255.0;
	c.a = a / 255.0;
	return c;
}

static inline Color color_from_hsv(f32 h, f32 s, f32 v, f32 a) {
	h = h - FLOOR(h);
	s = CLAMP(s, 0.0, 1.0);
	v = CLAMP(v, 0.0, 1.0);

	if (s == 0.0) return
		color_from_components(v, v, v, a);

	f32 hh = h * 6.0;
	if (hh >= 6.0) hh = 0.0;
	int i = (int)hh;
	f32 ff = hh - i;

	f32 p = v * (1.0 - s);
	f32 q = v * (1.0 - (s * ff));
	f32 t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
		case 0: return color_from_components(v, t, p, a);
		case 1: return color_from_components(q, v, p, a);
		case 2: return color_from_components(p, v, t, a);
		case 3: return color_from_components(p, q, v, a);
		case 4: return color_from_components(t, p, v, a);
		default: return color_from_components(v, p, q, a);
	}
}

static inline Color color_from_html(const char* html) {
	if (html[0] == '\0') return color_from_rgba8(0, 0, 0, 255);
	if (html[0] == '#') html++;

	size_t length = strlen(html);
	unsigned int r = 0, g = 0, b = 0, a = 255;

	if (length == 3 || length == 4) {
		sscanf(html, (length == 3)
			? "%1x%1x%1x"
			: "%1x%1x%1x%1x",
			&r, &g, &b, &a);
		r = (r << 4) | r;
		g = (g << 4) | g;
		b = (b << 4) | b;
		if (length == 4) a = (a << 4) | a;
	} else if (length == 6 || length == 8) {
		sscanf(html, (length == 6)
			? "%2x%2x%2x"
			: "%2x%2x%2x%2x",
			&r, &g, &b, &a);
	} else return color_from_rgba8(0, 0, 0, 255);

	return color_from_rgba8(r, g, b, a);
}

/* ========================================================================= */
/* Properties                                                                */
/* ========================================================================= */

static inline u8 color_get_r8(Color c) {
	return U8(CLAMP(c.r, 0.0, 1.0) * 255.0 + 0.5);
}

static inline u8 color_get_g8(Color c) {
	return U8(CLAMP(c.g, 0.0, 1.0) * 255.0 + 0.5);
}

static inline u8 color_get_b8(Color c) {
	return U8(CLAMP(c.b, 0.0, 1.0) * 255.0 + 0.5);
}

static inline u8 color_get_a8(Color c) {
	return U8(CLAMP(c.a, 0.0, 1.0) * 255.0 + 0.5);
}

static inline f32 color_min(Color c) {
	return MIN(MIN(c.r, c.g), c.b);
}

static inline u8 color_min8(Color c) {
	return MIN(MIN(
		color_get_r8(c),
		color_get_g8(c)),
		color_get_b8(c));
}

static inline f32 color_max(Color c) {
	return MAX(MAX(c.r, c.g), c.b);
}

static inline u8 color_max8(Color c) {
	return MAX(MAX(
		color_get_r8(c),
		color_get_g8(c)),
		color_get_b8(c));
}

static inline f32 color_get_luminance(Color c) {
	return 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
}

static inline f32 color_get_v(Color c) {
	return color_max(c);
}

static inline f32 color_get_s(Color c) {
	f32 max_c = color_max(c);
	f32 min_c = color_min(c);
	f32 delta = max_c - min_c;
	if (max_c == 0.0) return 0.0;
	return delta / max_c;
}

static inline f32 color_get_h(Color c) {
	f32 max_c = color_max(c);
	f32 min_c = color_min(c);
	f32 delta = max_c - min_c;
	if (delta == 0.0) return 0.0;

	f32 h = 0.0;
	if (max_c == c.r) {
		h = (c.g - c.b) / delta + (c.g < c.b ? 6.0 : 0.0);
	} else if (max_c == c.g) {
		h = (c.b - c.r) / delta + 2.0;
	} else {
		h = (c.r - c.g) / delta + 4.0;
	}

	return h / 6.0;
}

static inline bool color_is_equal(Color c1, Color c2, f32 epsilon) {
	return
		ABS(c1.r - c2.r) < epsilon &&
		ABS(c1.g - c2.g) < epsilon &&
		ABS(c1.b - c2.b) < epsilon &&
		ABS(c1.a - c2.a) < epsilon;
}

#define Color_equals color_equals
static inline bool color_equals(Color c1, Color c2) {
	return
		ABS(c1.r - c2.r) < COLOR_EPSILON &&
		ABS(c1.g - c2.g) < COLOR_EPSILON &&
		ABS(c1.b - c2.b) < COLOR_EPSILON &&
		ABS(c1.a - c2.a) < COLOR_EPSILON;
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static inline Color color_unpack(u32 c) {
	return color_from_components(
		((c >> 24) & 0xFF) / 255.0,
		((c >> 16) & 0xFF) / 255.0,
		((c >>  8) & 0xFF) / 255.0,
		(c & 0xFF) / 255.0);
}

static inline u32 color_pack(Color c) {
	return
		(U32(color_get_r8(c)) << 24) |
		(U32(color_get_g8(c)) << 16) |
		(U32(color_get_b8(c)) << 8) |
		(U32(color_get_a8(c)));
}

static inline Color color_lerp(Color c1, Color c2, f32 t) {
	return color_from_components(
		c1.r * (1.0 - t) + c2.r * t,
		c1.g * (1.0 - t) + c2.g * t,
		c1.b * (1.0 - t) + c2.b * t,
		c1.a * (1.0 - t) + c2.a * t);
}

static inline Color color_hsv_lerp(Color c1, Color c2, f32 t) {
	f32 h1 = color_get_h(c1);
	f32 s1 = color_get_s(c1);
	f32 v1 = color_get_v(c1);

	f32 h2 = color_get_h(c2);
	f32 s2 = color_get_s(c2);
	f32 v2 = color_get_v(c2);

	f32 dh = h2 - h1;
	if (dh > 0.5) {
		dh -= 1.0;
	} else if (dh < -0.5) {
		dh += 1.0;
	}

	f32 h = h1 + dh * t;
	h = h - FLOOR(h);

	f32 s = s1 * (1.0 - t) + s2 * t;
	f32 v = v1 * (1.0 - t) + v2 * t;
	f32 a = c1.a * (1.0 - t) + c2.a * t;

	return color_from_hsv(h, s, v, a);
}

static inline void color_to_html(Color c, char* html, bool alpha) {
	u8 r = color_get_r8(c);
	u8 g = color_get_g8(c);
	u8 b = color_get_b8(c);
	u8 a = color_get_a8(c);

	if (alpha) {
		snprintf(html, 10, "#%02x%02x%02x%02x", r, g, b, a);
	} else snprintf(html, 8, "#%02x%02x%02x", r, g, b);
}

static inline Color color_invert(Color c) {
	return color_from_components(
		1.0 - c.r,
		1.0 - c.g,
		1.0 - c.b,
		c.a);
}

static inline Color color_lighten(Color c, f32 amount) {
	return color_from_components(
		CLAMP(c.r + (1.0 - c.r) * amount, 0.0, 1.0),
		CLAMP(c.g + (1.0 - c.g) * amount, 0.0, 1.0),
		CLAMP(c.b + (1.0 - c.b) * amount, 0.0, 1.0),
		c.a);
}

static inline Color color_darken(Color c, f32 amount) {
	return color_from_components(
		CLAMP(c.r * (1.0 - amount), 0.0, 1.0),
		CLAMP(c.g * (1.0 - amount), 0.0, 1.0),
		CLAMP(c.b * (1.0 - amount), 0.0, 1.0),
		c.a);
}

static inline Color color_blend(Color base, Color over) {
	if (over.a >= 1.0) return over;
	if (over.a <= 0.0) return base;

	f32 a = over.a + base.a * (1.0 - over.a);
	if (a == 0.0) return color_from_rgba8(0, 0, 0, 0);

	return color_from_components(
		(over.r * over.a + base.r * base.a * (1.0 - over.a)) / a,
		(over.g * over.a + base.g * base.a * (1.0 - over.a)) / a,
		(over.b * over.a + base.b * base.a * (1.0 - over.a)) / a,
		a);
}

static inline Color color_desaturate(Color c, f32 factor) {
	f32 l = color_get_luminance(c);
	return color_from_components(
		c.r + (l - c.r) * factor,
		c.g + (l - c.g) * factor,
		c.b + (l - c.b) * factor,
		c.a);
}

/* ========================================================================= */
/* Hashing                                                                   */
/* ========================================================================= */

#define Color_hash color_hash
static inline usize color_hash(Color c) {
	return USIZE(hash_f32_mix4(c.r, c.g, c.b, c.a));
}

#endif /* MATH_COLOR_H */