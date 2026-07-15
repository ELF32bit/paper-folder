#ifndef MATH_COLOR_H
#define MATH_COLOR_H

#include "definitions/types.h"
#include "definitions/math.h"

#include <stdio.h>
#include <string.h>

#define COLOR_EPSILON 1e-3

typedef struct Color {
	union {
		struct { real r, g, b, a; };
		real components[4];
	};
} Color;

static inline Color color_from_rgba8(u8 r, u8 g, u8 b, u8 a) {
	return (Color){
		.r = r / 255.0,
		.g = g / 255.0,
		.b = b / 255.0,
		.a = a / 255.0
	};
}

static inline u8 color_get_r8(Color c) {
	return (u8)(CLAMP(c.r, 0.0, 1.0) * 255.0 + 0.5);
}

static inline u8 color_get_g8(Color c) {
	return (u8)(CLAMP(c.g, 0.0, 1.0) * 255.0 + 0.5);
}

static inline u8 color_get_b8(Color c) {
	return (u8)(CLAMP(c.b, 0.0, 1.0) * 255.0 + 0.5);
}

static inline u8 color_get_a8(Color c) {
	return (u8)(CLAMP(c.a, 0.0, 1.0) * 255.0 + 0.5);
}

static inline real color_min(Color c) {
	return MIN(MIN(c.r, c.g), c.b);
}

static inline u8 color_min8(Color c) {
	return MIN(MIN(
		color_get_r8(c),
		color_get_g8(c)),
		color_get_b8(c));
}

static inline real color_max(Color c) {
	return MAX(MAX(c.r, c.g), c.b);
}

static inline u8 color_max8(Color c) {
	return MAX(MAX(
		color_get_r8(c),
		color_get_g8(c)),
		color_get_b8(c));
}

static inline Color color_unpack(u32 c) {
	return (Color){
		.r = ((c >> 24) & 0xFF) / 255.0,
		.g = ((c >> 16) & 0xFF) / 255.0,
		.b = ((c >>  8) & 0xFF) / 255.0,
		.a = (c & 0xFF) / 255.0
	};
}

static inline u32 color_pack(Color c) {
	return
		((u32)color_get_r8(c) << 24) |
		((u32)color_get_g8(c) << 16) |
		((u32)color_get_b8(c) <<  8) |
		(u32)color_get_a8(c);
}

static inline real color_get_luminance(Color c) {
	return 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
}

static inline real color_get_v(Color c) {
	return color_max(c);
}

static inline real color_get_s(Color c) {
	real max_c = color_max(c);
	real min_c = color_min(c);
	real delta = max_c - min_c;
	if (max_c == 0.0) return 0.0;
	return delta / max_c;
}

static inline real color_get_h(Color c) {
	real max_c = color_max(c);
	real min_c = color_min(c);
	real delta = max_c - min_c;
	if (delta == 0.0) return 0.0;

	real h = 0.0;
	if (max_c == c.r) {
		h = (c.g - c.b) / delta + (c.g < c.b ? 6.0 : 0.0);
	} else if (max_c == c.g) {
		h = (c.b - c.r) / delta + 2.0;
	} else {
		h = (c.r - c.g) / delta + 4.0;
	}

	return h / 6.0;
}

static inline Color color_from_hsv(real h, real s, real v, real a) {
	h = h - FLOOR(h);
	s = CLAMP(s, 0.0, 1.0);
	v = CLAMP(v, 0.0, 1.0);

	if (s == 0.0) return (Color){ .components = { v, v, v, a } };

	real hh = h * 6.0;
	if (hh >= 6.0) hh = 0.0;
	int i = (int)hh;
	real ff = hh - i;

	real p = v * (1.0 - s);
	real q = v * (1.0 - (s * ff));
	real t = v * (1.0 - (s * (1.0 - ff)));

	switch (i) {
		case 0: return (Color){ .components = { v, t, p, a } };
		case 1: return (Color){ .components = { q, v, p, a } };
		case 2: return (Color){ .components = { p, v, t, a } };
		case 3: return (Color){ .components = { p, q, v, a } };
		case 4: return (Color){ .components = { t, p, v, a } };
		default: return (Color){ .components = { v, p, q, a } };
	}
}

static inline Color color_hsv_lerp(Color c1, Color c2, real t) {
	real h1 = color_get_h(c1);
	real s1 = color_get_s(c1);
	real v1 = color_get_v(c1);

	real h2 = color_get_h(c2);
	real s2 = color_get_s(c2);
	real v2 = color_get_v(c2);

	real dh = h2 - h1;
	if (dh > 0.5) {
		dh -= 1.0;
	} else if (dh < -0.5) {
		dh += 1.0;
	}

	real h = h1 + dh * t;
	h = h - FLOOR(h);

	real s = s1 * (1.0 - t) + s2 * t;
	real v = v1 * (1.0 - t) + v2 * t;
	real a = c1.a * (1.0 - t) + c2.a * t;

	return color_from_hsv(h, s, v, a);
}

static inline Color color_lerp(Color c1, Color c2, real t) {
	return (Color){
		.r = c1.r * (1.0 - t) + c2.r * t,
		.g = c1.g * (1.0 - t) + c2.g * t,
		.b = c1.b * (1.0 - t) + c2.b * t,
		.a = c1.a * (1.0 - t) + c2.a * t
	};
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

static inline Color color_from_html(const char* html) {
	if (html[0] == '\0') return color_from_rgba8(0, 0, 0, 255);
	if (html[0] == '#') html++;

	size_t len = strlen(html);
	unsigned int r = 0, g = 0, b = 0, a = 255;

	if (len == 3 || len == 4) {
		sscanf(html, (len == 3)
			? "%1x%1x%1x"
			: "%1x%1x%1x%1x",
			&r, &g, &b, &a);
		r = (r << 4) | r;
		g = (g << 4) | g;
		b = (b << 4) | b;
		if (len == 4) a = (a << 4) | a;
	} else if (len == 6 || len == 8) {
		sscanf(html, (len == 6)
			? "%2x%2x%2x"
			: "%2x%2x%2x%2x",
			&r, &g, &b, &a);
	} else return color_from_rgba8(0, 0, 0, 255);

	return color_from_rgba8(r, g, b, a);
}

static inline Color color_invert(Color c) {
	return (Color){
		.r = 1.0 - c.r,
		.g = 1.0 - c.g,
		.b = 1.0 - c.b,
		.a = c.a
	};
}

static inline Color color_lighten(Color c, real amount) {
	return (Color){
		.r = CLAMP(c.r + (1.0 - c.r) * amount, 0.0, 1.0),
		.g = CLAMP(c.g + (1.0 - c.g) * amount, 0.0, 1.0),
		.b = CLAMP(c.b + (1.0 - c.b) * amount, 0.0, 1.0),
		.a = c.a
	};
}

static inline Color color_darken(Color c, real amount) {
	return (Color){
		.r = CLAMP(c.r * (1.0 - amount), 0.0, 1.0),
		.g = CLAMP(c.g * (1.0 - amount), 0.0, 1.0),
		.b = CLAMP(c.b * (1.0 - amount), 0.0, 1.0),
		.a = c.a
	};
}

static inline Color color_blend(Color base, Color over) {
	if (over.a >= 1.0) return over;
	if (over.a <= 0.0) return base;

	real a = over.a + base.a * (1.0 - over.a);
	if (a == 0.0) return color_from_rgba8(0, 0, 0, 0);

	return (Color){
		.r = (over.r * over.a + base.r * base.a * (1.0 - over.a)) / a,
		.g = (over.g * over.a + base.g * base.a * (1.0 - over.a)) / a,
		.b = (over.b * over.a + base.b * base.a * (1.0 - over.a)) / a,
		.a = a
	};
}

static inline Color color_desaturate(Color c, real factor) {
	real l = color_get_luminance(c);
	return (Color){
		.r = c.r + (l - c.r) * factor,
		.g = c.g + (l - c.g) * factor,
		.b = c.b + (l - c.b) * factor,
		.a = c.a
	};
}

static inline bool color_is_equal(Color c1, Color c2) {
	return
		ABS(c1.r - c2.r) < COLOR_EPSILON &&
		ABS(c1.g - c2.g) < COLOR_EPSILON &&
		ABS(c1.b - c2.b) < COLOR_EPSILON &&
		ABS(c1.a - c2.a) < COLOR_EPSILON;
}

#endif /* MATH_COLOR_H */