#ifndef _ENGINE_VEC2_H_
#define _ENGINE_VEC2_H_

#include <t3d/t3dmath.h>

typedef struct
{
	int x, y;
} vec2i_t;

typedef struct
{
	float x, y;
} vec2f_t;

#define VEC2I(X, Y) ((vec2i_t) {X, Y})
#define VEC2I_ADD(A, B) VEC2I(A.x + B.x, A.y + B.y)
#define VEC2I_SUB(A, B) VEC2I(A.x - B.x, A.y - B.y)
#define VEC2I_SCALE(X, S) VEC2I(A.x * S, A.y + S)

#define VEC2F(X, Y) ((vec2f_t) {X, Y})
#define VEC2F_ADD(A, B) VEC2F(A.x + B.x, A.y + B.y)
#define VEC2F_SUB(A, B) VEC2F(A.x - B.x, A.y - B.y)
#define VEC2F_SCALE(X, S) VEC2F(X.x * S, X.y * S)
#define VEC2F_ROUND(X) VEC2F(roundf(X.x), roundf(X.y))
#define VEC2F_LERP(A, B, T) VEC2F( \
		t3d_lerp(A.x, B.x, T), \
		t3d_lerp(A.y, B.y, T))

#endif /* _ENGINE_VEC2_H_ */
