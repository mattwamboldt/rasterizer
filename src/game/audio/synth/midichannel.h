#ifndef AUDIO_SYNTH_MIDICHANNEL_H
#define AUDIO_SYNTH_MIDICHANNEL_H

#include "midinote.h"

namespace Audio
{
	class MidiChannel
	{
	public:
		void Init();

		void SetVolume(float value);
		float GetVolume() { return volume; }

		void NoteOff(Uint8 note, Uint8 velocity = 64);
		void NoteOn(Uint8 note, Uint8 velocity = 64);
		void Aftertouch(Uint8 note, Uint8 pressure);

		void AllNotesOff();

		void Write(PCM16* data, int count);

	private:
		MidiNote notes[NUM_MIDI_NOTES];
		float volume;
		BreakpointFile attack;
		BreakpointFile decay;
		BreakpointFile release;
	};
}

#endif
