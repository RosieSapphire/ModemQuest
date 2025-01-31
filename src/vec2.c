#include <math.h>

#include "util.h"

#include "vec2.h"

/******************
 * FLOATING POINT *
 ******************/
void vec2f_zero(vec2f dst)
{
	dst[0] = 0.f;
	dst[1] = 0.f;
}

void vec2f_set(vec2f dst, const float x, const float y)
{
	dst[0] = x;
	dst[1] = y;
}

void vec2f_copy(vec2f dst, const vec2f src)
{
	dst[0] = src[0];
	dst[1] = src[1];
}

void vec2f_add(vec2f dst, const vec2f a, const vec2f b)
{
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
}

void vec2f_sub(vec2f dst, const vec2f a, const vec2f b)
{
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

void vec2f_lerp(vec2f dst, const vec2f a, const vec2f b, const float t)
{
	dst[0] = lerpf(a[0], b[0], t);
	dst[1] = lerpf(a[1], b[1], t);
}

void vec2f_round(vec2f dst)
{
	dst[0] = roundf(dst[0]);
	dst[1] = roundf(dst[1]);
}

void vec2f_scale(vec2f dst, const vec2f a, const float s)
{
	dst[0] = a[0] * s;
	dst[1] = a[1] * s;
}

/***********
 * INTEGER *
 ***********/
void vec2i_zero(vec2i dst)
{
	dst[0] = 0;
	dst[1] = 0;
}

void vec2i_set(vec2i dst, const int x, const int y)
{
	dst[0] = x;
	dst[1] = y;
}

void vec2i_copy(vec2i dst, const vec2i src)
{
	dst[0] = src[0];
	dst[1] = src[1];
}

void vec2i_add(vec2i dst, const vec2i a, const vec2i b)
{
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
}

void vec2i_sub(vec2i dst, const vec2i a, const vec2i b)
{
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

void vec2i_lerp(vec2i dst, const vec2i a, const vec2i b, const float t)
{
	dst[0] = lerpi(a[0], b[0], t);
	dst[1] = lerpi(a[1], b[1], t);
}
