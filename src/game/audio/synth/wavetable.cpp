#include "wavetable.h"
#include "../types.h"
#include <math.h>

namespace Audio
{
	double WaveTable::sampleIncrement = TABLELENGTH / 44100.0; //Assume a base rate of cd quality

	//For now we fill the table with a basic sin wave, later this will be altered to
	//allow complex harmonics and such
	WaveTable::WaveTable()
	{
		phase = 0.0;

		double step = TWO_PI / TABLELENGTH;
		for(Uint32 i = 0; i < TABLELENGTH; ++i)
		{
			table[i] = sin(step * i);
		}

		SetFrequency(440);
	}

	void WaveTable::SetSamplingRate(double sr)
	{
		sampleIncrement = TABLELENGTH / sr;
	}

	double WaveTable::NextSample()
	{
		double value = table[(int)phase];
		phase += increment;
		while(phase >= TABLELENGTH) phase -= TABLELENGTH;
		while(phase < 0) phase += TABLELENGTH;
		return value;
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