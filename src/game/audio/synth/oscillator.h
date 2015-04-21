#ifndef AUDIO_SYNTH_OSCILLATOR
#define AUDIO_SYNTH_OSCILLATOR

#include "../types.h"
#include "../file/breakpoint.h"

namespace Audio
{
	enum EnvelopeState
	{
		ENV_ATTACK,
		ENV_DECAY,
		ENV_SUSTAIN,
		ENV_RELEASE,
		ENV_NONE
	};

	class Oscillator
	{
	public:
		Oscillator();
		void SetVolume(double value){volume = value;}
		void Press(Uint8 velocity);
		void Release(Uint8 velocity);
		void Stop() { playing = false; }
		void SetFrequency(double frequency);
		void SetMidiNote(Uint8 note);
		void Write(PCM16* data, int count);

		void SetADSR(BreakpointFile* atk, BreakpointFile* dec, BreakpointFile* rel);

		static void SetScale(Uint32 numSemitones = 12, float refFrequency = 440.0f);

	private:
		float GetEnvelope();

		static double semitoneRatio;
		static double c0;

		double phase;
		double increment;
		double volume;

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
