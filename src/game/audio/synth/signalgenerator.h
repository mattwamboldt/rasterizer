#ifndef AUDIO_SYNTH_SIGNALGENERATOR
#define AUDIO_SYNTH_SIGNALGENERATOR

#include "../types.h"
#include "../file/breakpoint.h"
#include "../file/wave.h"

namespace Audio
{
    // The signal generator writes a preset tone from an oscillator to a wavefile
    // It allows us to generate single, simple waves from breakpoint data
    // In some respects this doesn't need to exist when we have the wavebank but
    // it's here for learning purposes
	class SignalGenerator
	{
	public:
		SignalGenerator()
			: frequencyMod(0), amplitudeEnv(0), pulseWidthMod(0),
			samplingRate(44100), duration(5.0f), tremoloFrequency(0.0)
		{}

		void WriteTone(WaveFile& file, WaveType type = SINE_WAVE);

		void SetDuration(float value){ duration = value; }
		void SetTremolo(float frequency){ tremoloFrequency = frequency; }
		void SetAmplitudeEnvelope(BreakpointFile* value) {amplitudeEnv = value;}
		void SetFrequencyModulation(BreakpointFile* value) {frequencyMod = value;}
		void SetPulseWidthModulation(BreakpointFile* value) {pulseWidthMod = value;}

		Uint32 samplingRate;

	private:
		float duration;
		double tremoloFrequency;

		BreakpointFile* frequencyMod;
		BreakpointFile* amplitudeEnv;
		BreakpointFile* pulseWidthMod;
	};
}

#endif