#ifndef _ENGINE_VEC2_H_
#define _ENGINE_VEC2_H_

typedef float vec2f[2];
typedef int vec2i[2];

#define VEC2_SET(DST, X, Y) \
	do {                \
		DST[0] = X; \
		DST[1] = Y; \
	} while (0)
#define VEC2_COPY(DST, SRC) VEC2_SET(DST, SRC[0], SRC[1])
#define VEC2_ZERO(DST) VEC2_SET(DST, 0, 0)

#define VEC2_ADD(DST, A, B)           \
	do {                          \
		DST[0] = A[0] + B[0]; \
		DST[1] = A[1] + B[1]; \
	} while (0)
#define VEC2_SUB(DST, A, B)           \
	do {                          \
		DST[0] = A[0] - B[0]; \
		DST[1] = A[1] - B[1]; \
	} while (0)
#define VEC2_SCALE(DST, X, S)      \
	do {                       \
		DST[0] = X[0] * S; \
		DST[1] = X[1] * S; \
	} while (0)
#define VEC2_ROUND(X)                \
	do {                         \
		X[0] = roundf(X[0]); \
		X[1] = roundf(X[1]); \
	} while (0)
#define VEC2_LERP(DST, A, B, T)                    \
	do {                                       \
		DST[0] = A[0] + (B[0] - A[0]) * T; \
		DST[1] = A[1] + (B[1] - A[1]) * T; \
	} while (0)

#endif /* _ENGINE_VEC2_H_ */
