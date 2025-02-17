#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "types.h"

/* SIGNED 16-BIT INT */
typedef s16 vec2s[2];
void vec2s_zero(vec2s dst);
void vec2s_set(vec2s dst, const s16 x, const s16 y);
void vec2s_copy(vec2s dst, const vec2s src);
void vec2s_add(vec2s dst, const vec2s a, const vec2s b);
void vec2s_sub(vec2s dst, const vec2s a, const vec2s b);
boolean vec2s_equals(const vec2s a, const vec2s b);

/* 32-BIT FLOAT */
typedef f32 vec2f[2];
void vec2f_zero(vec2f dst);
void vec2f_set(vec2f dst, const f32 x, const f32 y);
void vec2f_copy(vec2f dst, const vec2f src);
void vec2f_sub(vec2f dst, const vec2f a, const vec2f b);
void vec2f_lerp(vec2f dst, const vec2f a, const vec2f b, const f32 t);
void vec2f_round(vec2f dst, const vec2f src);
void vec2f_scale(vec2f dst, const vec2f src, const f32 scale);

#endif /* _VECTOR_H_ */
