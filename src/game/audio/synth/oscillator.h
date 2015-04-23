#ifndef AUDIO_SYNTH_OSCILLATOR
#define AUDIO_SYNTH_OSCILLATOR

#include "../types.h"

namespace Audio
{
	typedef double (*ValueGenerator)(double);

	class Oscillator
	{
	public:
		Oscillator();

		void Reset(){ phase = 0.0; }
		void SetFrequency(double frequency);
		void SetWaveType(WaveType value);
		double NextSample(double freq);
		double NextSample();

		static void SetSamplingRate(double sr);

		double GetFrequency(){ return frequency; };

	private:
		static double samplingRadians;

		double phase;
		double increment;
		double frequency;

		ValueGenerator waveFunction;
	};
}

#endif
