#include "wavetable.h"
#include "../types.h"
#include <math.h>

namespace Audio
{
	double WaveTable::sampleIncrement = TABLELENGTH / 44100.0; //Assume a base rate of cd quality

	WaveTable::WaveTable()
	{
		SDL_memset(table, 0, sizeof(double) * (TABLELENGTH + 1));
		phase = 0.0;
		SetFrequency(440);
	}

	void WaveTable::Normalize()
	{
		double maxAmp = 0.0;
		for (unsigned long i = 0; i < TABLELENGTH; ++i)
		{
			if (table[i] > maxAmp)
			{
				maxAmp = table[i];
			}
		}

		maxAmp = 1.0 / maxAmp;

		for (unsigned long i = 0; i < TABLELENGTH; ++i)
		{
			table[i] *= maxAmp;
		}

		table[TABLELENGTH] = table[0];
	}

	void WaveTable::SetSamplingRate(double sr)
	{
		sampleIncrement = TABLELENGTH / sr;
	}

	void WaveTable::SetVolume(double value)
	{
		if (value < 0.0f)
		{
			volume = 0.0f;
		}
		else if (value > 1.0f)
		{
			value = 1.0f;
		}
		else
		{
			volume = value;
		}
	}

	double WaveTable::NextSample()
	{
		//Linear interpolating wavetable
		int base = (int)phase;
		int next = base + 1;

		double t = phase - base;
		double value = table[base];
		double slope = table[next] - value;
		value += t * slope;

		phase += increment;
		while(phase >= TABLELENGTH) phase -= TABLELENGTH;
		while(phase < 0.0) phase += TABLELENGTH;

		return value * volume;
	}

	double WaveTable::NextSample(double freq)
	{
		if(frequency != freq)
		{
			SetFrequency(freq);
		}

		return NextSample();
	}

	void WaveTable::SetFrequency(double freq)
	{
		frequency = freq;
		increment = sampleIncrement * frequency;
	}
}