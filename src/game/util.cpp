#include "util.h"

double lerp(double v0, double v1, double t)
{
	return ((1-t)* v0) + (t * v1);
}

float lerp(float v0, float v1, float t)
{
	return ((1-t)* v0) + (t * v1);
}