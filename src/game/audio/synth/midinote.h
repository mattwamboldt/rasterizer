#ifndef AUDIO_SYNTH_MIDINOTE
#define AUDIO_SYNTH_MIDINOTE

#include "../types.h"
#include "../file/breakpoint.h"
#include "oscillator.h"

namespace Audio
{
	class MidiNote
	{
	public:
		MidiNote();
		void SetVolume(double value){volume = value;}
		void Press(Uint8 velocity);
		void Release(Uint8 velocity);
		void Stop() { playing = false; }
		
		void SetMidiNote(Uint8 note);
		void SetWaveType(WaveType value);

		void Write(PCM16* data, int count);

		void SetADSR(BreakpointFile* atk, BreakpointFile* dec, BreakpointFile* rel);

		static void SetScale(Uint32 numSemitones = 12, float refFrequency = 440.0f);

	private:
		float GetEnvelope();

		static double semitoneRatio;
		static double c0;

		double volume;
		Oscillator wave;

		bool playing;
		EnvelopeState currentState;
		EnvelopeState nextState;

		Uint32 sampleCount;
		BreakpointFile* attack;
		BreakpointFile* decay;
		BreakpointFile* release;
	};
}

#endif
