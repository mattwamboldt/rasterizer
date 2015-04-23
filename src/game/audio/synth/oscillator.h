#ifndef AUDIO_SYNTH_OSCILLATOR
#define AUDIO_SYNTH_OSCILLATOR

#include "../types.h"
#include "../file/breakpoint.h"

//TODO: Separate the oscillator from the midi implementation details

namespace Audio
{
	typedef double (*ValueGenerator)(double);

	enum EnvelopeState
	{
		ENV_ATTACK,
		ENV_DECAY,
		ENV_SUSTAIN,
		ENV_RELEASE,
		ENV_NONE
	};

	enum WaveType
	{
		SINE_WAVE,
		SQUARE_WAVE,
		TRIANGLE_WAVE,
		UPSAW_WAVE,
		DOWNSAW_WAVE,
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
		void SetWaveType(WaveType value);

		void Write(PCM16* data, int count);

		void SetADSR(BreakpointFile* atk, BreakpointFile* dec, BreakpointFile* rel);

		static void SetScale(Uint32 numSemitones = 12, float refFrequency = 440.0f);
		static void SetSamplingRate(double sr);

	private:
		float GetEnvelope();
		double NextSample(double freq);

		static double semitoneRatio;
		static double c0;
		static double samplingRadians;

		double phase;
		double increment;
		double volume;
		double frequency;

		ValueGenerator waveFunction;

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
