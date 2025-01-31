#ifndef _VEC2_H_
#define _VEC2_H_

typedef float vec2f[2];
typedef int vec2i[2];

void vec2f_zero(vec2f dst);
void vec2f_set(vec2f dst, const float x, const float y);
void vec2f_copy(vec2f dst, const vec2f src);
void vec2f_add(vec2f dst, const vec2f a, const vec2f b);
void vec2f_sub(vec2f dst, const vec2f a, const vec2f b);
void vec2f_lerp(vec2f dst, const vec2f a, const vec2f b, const float t);
void vec2f_round(vec2f dst);
void vec2f_scale(vec2f dst, const vec2f a, const float s);

void vec2i_zero(vec2i dst);
void vec2i_set(vec2i dst, const int x, const int y);
void vec2i_copy(vec2i dst, const vec2i src);
void vec2i_add(vec2i dst, const vec2i a, const vec2i b);
void vec2i_sub(vec2i dst, const vec2i a, const vec2i b);
void vec2i_lerp(vec2i dst, const vec2i a, const vec2i b, const float t);

#endif /* _VEC2_H_ */
