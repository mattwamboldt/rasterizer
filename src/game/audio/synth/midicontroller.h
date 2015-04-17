#ifndef AUDIO_SYNTH_MIDI
#define AUDIO_SYNTH_MIDI

#include "../types.h"
#include "oscillator.h"

#define NUM_MIDI_CHANNELS 16
#define NUM_MIDI_NOTES 128

namespace Audio
{
	//This class defines a software emulation of a full midi system
	//It responds to events and spits out audio
	class MidiController
	{
	public:
		void Init();
		void ProcessEvent(const MidiEvent& e);
		void Write(PCM16* data, int count);
	private:
		Audio::Oscillator notes[NUM_MIDI_CHANNELS][NUM_MIDI_NOTES];
	};
}

#endif
