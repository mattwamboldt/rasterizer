#include "oscillator.h"
#include <math.h>

#define PI     3.14159265359
#define TWO_PI 6.28318530718

namespace Audio
{
	double sine_wave(double phase)
	{
		return sin(phase);
	}

	double square_wave(double phase)
	{
		if(phase <= PI)
		{
			return 1.0;
		}
		else
		{
			return -1.0;
		}
	}

	double downward_sawtooth_wave(double phase)
	{
		return 1.0 - 2.0 * (phase / TWO_PI);
	}

	double upward_sawtooth_wave(double phase)
	{
		return 2.0 * (phase / TWO_PI) - 1.0;
	}

	double triangle_wave(double phase)
	{
		double result = upward_sawtooth_wave(phase);
		if(result < 0.0) result = -result;
		return 2.0 * (result - 0.5);
	}

	Oscillator::Oscillator() : phase(0.0), volume(0.5)
	{
		SetFrequency(440);
	}

	void Oscillator::SetFrequency(double frequency)
	{
		increment = frequency / audioSpec.freq * TWO_PI;
	}

	void Oscillator::Write(PCM16* data, int count)
	{
		for(int i = 0; i < count; i +=2)
		{
			double value = sine_wave(phase) * 32767.0 * volume;
			data[i] = value;
			data[i+1] = value;

			phase += increment;

			if(phase >= TWO_PI)
			{
				phase -= TWO_PI;
			}
		}
	}
}
