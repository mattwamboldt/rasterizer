#ifndef AUDIO_SYNTH_OSCILLATORBANK
#define AUDIO_SYNTH_OSCILLATORBANK

#include "../types.h"
#include "oscillator.h"
#include "../file/breakpoint.h"
#include "../file/wave.h"
#include <vector>

namespace Audio
{
	class OscillatorBank
	{
	public:
		OscillatorBank()
			: frequencyMod(0), amplitudeEnv(0), samplingRate(44100), duration(5.0f)
		{}
		
		void Init(Uint32 numsignals, WaveType type = SQUARE_WAVE);

		void WriteTone(WaveFile& file);

		void SetDuration(float value){ duration = value; }
		void SetAmplitudeEnvelope(BreakpointFile* value) {amplitudeEnv = value;}
		void SetFrequencyModulation(BreakpointFile* value) {frequencyMod = value;}

		Uint32 samplingRate;

	private:
		float duration;

		Uint32 numOscilltors;
		std::vector<Oscillator> oscillators;
		std::vector<double> amplitudes;
		std::vector<double> frequencies;

		BreakpointFile* frequencyMod;
		BreakpointFile* amplitudeEnv;
	};
}

#endif
