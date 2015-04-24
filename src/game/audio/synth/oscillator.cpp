#include "oscillator.h"
#include <math.h>
#include "../audio.h"
#include "../../debug.h"

#define PI     3.14159265359
#define TWO_PI 6.28318530718

namespace Audio
{
	double Oscillator::samplingRadians = TWO_PI / 44100.0; //Assume a base rate of cd quality

	double sin_wave(double phase, double pulseWidth)
	{
		return sin(phase);
	}

	double square_wave(double phase, double pulseWidth)
	{
		if(phase <= TWO_PI * pulseWidth)
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
		if(result < 0.0) result = -result;
		return 2.0 * (result - 0.5);
	}

	Oscillator::Oscillator() : phase(0.0), pulseWidth(0.5)
	{
		SetFrequency(440);
		SetWaveType(SINE_WAVE);
	}

	void Oscillator::SetPulseWidth(double pw)
	{
		if(pw < 0.01)
		{
			pulseWidth = 0.01;
		}
		else if( pw > 0.99)
		{
			pulseWidth = 0.99;
		}
		else
		{
			pulseWidth = pw;
		}
	}

	void Oscillator::SetSamplingRate(double sr)
	{
		samplingRadians = TWO_PI / sr;
	}

	void Oscillator::SetWaveType(WaveType value)
	{
		switch(value)
		{
		case SQUARE_WAVE:
			waveFunction = &square_wave;
			break;

		case UPSAW_WAVE:
			waveFunction = &upward_sawtooth_wave;
			break;

		case DOWNSAW_WAVE:
			waveFunction = &downward_sawtooth_wave;
			break;

		case TRIANGLE_WAVE:
			waveFunction = &triangle_wave;
			break;

		case SINE_WAVE:
		default:
			waveFunction = &sin_wave;
			break;
		}
	}

	void Oscillator::SetFrequency(double freq)
	{
		frequency = freq;
		increment = frequency * samplingRadians;
	}

	double Oscillator::NextSample(double freq)
	{
		if(freq != frequency)
		{
			SetFrequency(freq);
		}

		return NextSample();
	}

	double Oscillator::NextSample()
	{
		double value = waveFunction(phase, pulseWidth);

		phase += increment;

		if(phase >= TWO_PI)
		{
			phase -= TWO_PI;
		}

		if(phase < 0.0)
		{
			phase += TWO_PI;
		}

		return value;
	}
}
