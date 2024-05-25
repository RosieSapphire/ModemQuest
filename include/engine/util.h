#ifndef _ENGINE_UTIL_H_
#define _ENGINE_UTIL_H_

#include <stdint.h>

#define UNUSED __attribute__((unused))

#define ABS(X) ((X < 0) ? X * -1 : X)
#define MIN(X, MINV) (X < MINV ? MINV : X)
#define MAX(X, MAXV) (X > MAXV ? MAXV : X)
#define CLAMP(X, MINV, MAXV) (MAX(MIN(X, MINV), MAXV))

float lerpf(const float a, const float b, const float t);
void rdpq_fill_rect_border(const int x0, const int y0,
			   const int x1, const int y1,
			   const uint8_t r, const uint8_t g, const uint8_t b,
			   const uint8_t thick);

#endif /* _ENGINE_UTIL_H_ */
