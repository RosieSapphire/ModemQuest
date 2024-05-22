#ifndef _ENGINE_UTIL_H_
#define _ENGINE_UTIL_H_

#define MIN(X, MINV) (X < MINV ? MINV : X)
#define MAX(X, MAXV) (X > MAXV ? MAXV : X)
#define CLAMP(X, MINV, MAXV) (MAX(MIN(X, MINV), MAXV))

float lerpf(const float a, const float b, const float t);

#endif /* _ENGINE_UTIL_H_ */
