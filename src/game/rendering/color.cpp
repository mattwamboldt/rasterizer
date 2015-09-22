#include "color.h"

Color blendAverage(const Color& source, Color& target)
{
	Color ret;
	ret.r = ((Uint32)source.r + (Uint32)target.r) >> 1;
	ret.g = ((Uint32)source.g + (Uint32)target.g) >> 1;
	ret.b = ((Uint32)source.b + (Uint32)target.b) >> 1;
	return ret;
}

Color blendMultiply(const Color& source, Color& target)
{
	Color ret;
	ret.r = ((Uint32)source.r * (Uint32)target.r) >> 8;
	ret.g = ((Uint32)source.g * (Uint32)target.g) >> 8;
	ret.b = ((Uint32)source.b * (Uint32)target.b) >> 8;
	return ret;
}

Color blendAdd(const Color& source, Color& target)
{
	Color ret;
	ret.r = SDL_max((Uint32)source.r + (Uint32)target.r, 255);
	ret.g = SDL_max((Uint32)source.g + (Uint32)target.g, 255);
	ret.b = SDL_max((Uint32)source.b + (Uint32)target.b, 255);
	return ret;
}