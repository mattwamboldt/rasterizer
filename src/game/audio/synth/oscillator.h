#ifndef AUDIO_SYNTH_OSCILLATOR
#define AUDIO_SYNTH_OSCILLATOR

#include "../audio.h"

namespace Audio
{
	class Oscillator
	{
	public:
		Oscillator();
		void SetVolume(double value){volume = value;}
		void SetFrequency(double frequency);
		void Write(PCM16* data, int count);

	private:
		double phase;
		double increment;
		double volume;
	};
}

#endif
