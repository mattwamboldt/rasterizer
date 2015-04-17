#include "midicontroller.h"

namespace Audio
{
	void MidiController::Init()
	{
		for(int channel = 0; channel < NUM_MIDI_CHANNELS; ++channel)
		{
			for(int note = 0; note < NUM_MIDI_NOTES; ++note)
			{
				notes[channel][note].SetMidiNote(note);
			}
		}
	}

	void MidiController::ProcessEvent(const MidiEvent& e)
	{
		int command = e.eventCode >> 4;
		int channel = e.eventCode & 0x0F;
		if(command == MIDI_NOTE_ON)
		{
			notes[channel][e.param1].Play(e.param2);
		}
		else if(command == MIDI_NOTE_OFF)
		{
			notes[channel][e.param1].Stop(e.param2);
		}
	}

	void MidiController::Write(PCM16* data, int count)
	{
		for(int channel = 0; channel < NUM_MIDI_CHANNELS; ++channel)
		{
			for(int note = 0; note < NUM_MIDI_NOTES; ++note)
			{
				notes[channel][note].Write(data, count);
			}
		}
	}
}