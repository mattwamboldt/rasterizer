#ifndef AUDIO_SYNTH_MIDI
#define AUDIO_SYNTH_MIDI

#include "../types.h"
#include "oscillator.h"

#define NUM_MIDI_CHANNELS 16
#define NUM_MIDI_NOTES 128

namespace Audio
{
	class MidiFile;
	//This class defines a software emulation of a full midi system
	//It responds to events and spits out audio
	class MidiController
	{
	public:
		void Init(MidiFile* file = 0);
		void ProcessEvent(const MidiEvent& e);
		void Write(PCM16* data, int count);
	private:
		void RefreshPulseRate();
		void ControlChange(Uint8 channel, Uint8 controller, Uint8 value);
		void SystemEvent(Uint8 systemMessage, Uint32 p1, Uint32 p2);

		void AllNotesOff(Uint8 channel);

		Audio::Oscillator notes[NUM_MIDI_CHANNELS][NUM_MIDI_NOTES];
		float channelVolumes[NUM_MIDI_CHANNELS];

		//Timers used to clock the event pump
		Uint32 timePerBeat; //measured in microseconds
		float timePerSample;
		float timePerPulse;
		float timeSincePulse;
		MidiFile* midiFile;
	};
}

#endif
