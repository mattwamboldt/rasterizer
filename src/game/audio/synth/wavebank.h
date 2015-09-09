#ifndef AUDIO_SYNTH_WAVEBANK_H
#define AUDIO_SYNTH_WAVEBANK_H

#include "..\types.h"
#include "..\file\breakpoint.h"
#include "..\file\wave.h"
#include "..\effects\tremolo.h"
#include "wavetable.h"
#include <vector>
using namespace std;

namespace Audio
{
	class WaveBank
	{
	public:
		WaveBank()
			: frequencyMod(0), amplitudeEnv(0),
			samplingRate(44100), duration(5.0f),
			tremolo(0.3, 0.0), vibratoFrequency(0.0)
		{}

		void AddWave(WaveType waveType, double harmonicity = 1.0, double frequency = 440.0, double volume = 1.0);
		void Write(const char* path);
		void Write(WaveFile& file);
		void Write(PCM16* buffer, int count);

		void SetDuration(float value){ duration = value; }
		void SetTremolo(float frequency){ tremolo.SetFrequency(frequency); }
		void SetVibrato(float frequency){ vibratoFrequency = frequency; }
		void SetAmplitudeEnvelope(BreakpointFile* value) { amplitudeEnv = value; }
		void SetFrequencyModulation(BreakpointFile* value) { frequencyMod = value; }

		Uint32 samplingRate;

	private:
		float duration;
		Tremolo tremolo;
		double vibratoFrequency;

		vector<WaveTable> waves;
		BreakpointFile* frequencyMod;
		BreakpointFile* amplitudeEnv;
	};
}

#endif