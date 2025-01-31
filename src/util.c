#include <math.h>

#include "util.h"

float lerpf(const float a, const float b, const float t)
{
	return a + (b - a) * t;
}

int lerpi(const int a, const int b, const float t)
{
	return (int)roundf(lerpf((float)a, (float)b, t));
}
