#ifndef AUDIO_SYNTH_OSCILLATOR
#define AUDIO_SYNTH_OSCILLATOR

#include "../types.h"

namespace Audio
{
	//Represents a purely mathematical Oscillator which generates
	//values according to a wave function, has issues with speed and noise at
	//high frequencies for some of the wave functions
	class Oscillator
	{
	public:
		Oscillator();

		void Reset(double startphase = 0.0){ phase = TWO_PI * startphase; }
		void SetFrequency(double frequency);
		void SetWaveType(WaveType value);
		void SetPulseWidth(double pw);

		double NextSample(double freq);
		double NextSample();

		static void SetSamplingRate(double sr);

		double GetFrequency(){ return frequency; };

	private:
		static double samplingRadians;

		double phase;
		double increment;
		double frequency;
		double pulseWidth;

		ValueGenerator waveFunction;
	};
}

#endif
