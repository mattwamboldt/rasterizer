#ifndef AUDIO_SYNTH_OSCILLATOR
#define AUDIO_SYNTH_OSCILLATOR

#include "../types.h"

namespace Audio
{
	class Oscillator
	{
	public:
		Oscillator();
		void SetVolume(double value){volume = value;}
		void Play(Uint8 velocity);
		void Stop(Uint8 velocity) { playing = false; }
		void SetFrequency(double frequency);
		void SetMidiNote(Uint8 note);
		void Write(PCM16* data, int count);

	private:
		double phase;
		double increment;
		double volume;
		bool playing;
	};
}

#endif
