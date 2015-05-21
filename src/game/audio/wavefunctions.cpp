#include "wavefunctions.h"
#include "types.h"
#include <math.h>

namespace Audio
{
	double sin_wave(double phase, double pulseWidth)
	{
		return sin(phase);
	}

	double cos_wave(double phase, double pulseWidth)
	{
		return cos(phase);
	}

	double square_wave(double phase, double pulseWidth)
	{
		if (phase <= TWO_PI * pulseWidth)
		{
			return 1.0;
		}

		return -1.0;
	}

	double downward_sawtooth_wave(double phase, double pulseWidth)
	{
		return 1.0 - 2.0 * (phase / TWO_PI);
	}

	double upward_sawtooth_wave(double phase, double pulseWidth)
	{
		return 2.0 * (phase / TWO_PI) - 1.0;
	}

	double triangle_wave(double phase, double pulseWidth)
	{
		double result = upward_sawtooth_wave(phase, pulseWidth);
		if (result < 0.0) result = -result;
		return 2.0 * (result - 0.5);
	}
}