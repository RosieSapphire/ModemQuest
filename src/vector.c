#include <math.h>

#include "util.h"
#include "vector.h"

/* SIGNED 16-BIT INT */
void vec2s_zero(vec2s dst)
{
	dst[0] = 0;
	dst[1] = 0;
}

void vec2s_set(vec2s dst, const s16 x, const s16 y)
{
	dst[0] = x;
	dst[1] = y;
}

void vec2s_copy(vec2s dst, const vec2s src)
{
	dst[0] = src[0];
	dst[1] = src[1];
}

void vec2s_add(vec2s dst, const vec2s a, const vec2s b)
{
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
}

void vec2s_sub(vec2s dst, const vec2s a, const vec2s b)
{
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

boolean vec2s_equals(const vec2s a, const vec2s b)
{
	return (a[0] == b[0] && a[1] == b[1]);
}

/* 32-BIT FLOAT */
void vec2f_zero(vec2f dst)
{
	dst[0] = 0;
	dst[1] = 0;
}

void vec2f_set(vec2f dst, const f32 x, const f32 y)
{
	dst[0] = x;
	dst[1] = y;
}

void vec2f_copy(vec2f dst, const vec2f src)
{
	dst[0] = src[0];
	dst[1] = src[1];
}

void vec2f_sub(vec2f dst, const vec2f a, const vec2f b)
{
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

void vec2f_lerp(vec2f dst, const vec2f a, const vec2f b, const f32 t)
{
	dst[0] = lerpf(a[0], b[0], t);
	dst[1] = lerpf(a[1], b[1], t);
}

void vec2f_round(vec2f dst, const vec2f src)
{
	dst[0] = roundf(src[0]);
	dst[1] = roundf(src[1]);
}

void vec2f_scale(vec2f dst, const vec2f src, const f32 scale)
{
	dst[0] = src[0] * scale;
	dst[1] = src[1] * scale;
}
