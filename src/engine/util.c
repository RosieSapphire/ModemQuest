#include "engine/util.h"

float lerpf(const float a, const float b, const float t)
{
	return (a + (b - a) * t);
}
