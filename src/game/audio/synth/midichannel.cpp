#include "midichannel.h"
#include "../audio.h"

namespace Audio
{
	void MidiChannel::Init()
	{
		attack.GenerateCurve(0.1, audioSpec.freq * 0.1, 0.0, 1.0);
		decay.GenerateCurve(0.1, audioSpec.freq * 0.1, 1.0, 0.5);
		release.GenerateCurve(0.1, audioSpec.freq * 0.25, 0.5, 0.0);
		SetVolume(0.5f);
		for(int note = 0; note < NUM_MIDI_NOTES; ++note)
		{
			notes[note].SetMidiNote(note);
			notes[note].SetADSR(&attack, &decay, &release);
		}
	}

	void MidiChannel::SetVolume(float value)
	{
		if(value < 0.0f)
		{
			volume = 0.0f;
		}
		else if(value > 1.0f)
		{
			value = 1.0f;
		}
		else
		{
			volume = value;
		}

		for(int note = 0; note < NUM_MIDI_NOTES; ++note)
		{
			//TODO: not the best way to do this, should be set in one place, not per oscillator
			notes[note].SetVolume(volume);
		}
	}

	void MidiChannel::NoteOn(Uint8 note, Uint8 velocity)
	{
		if(velocity == 0)
		{
			NoteOff(note, 64); //This is a hack the midi spec uses to abuse running status for mild compression
		}
		else
		{
			notes[note].Press(velocity); //TODO: should be a press with an attack
		}
	}

	void MidiChannel::NoteOff(Uint8 note, Uint8 velocity)
	{
		notes[note].Release(velocity); //TODO: should be a release with a decay
	}

	void MidiChannel::Aftertouch(Uint8 note, Uint8 pressure)
	{

	}

	void MidiChannel::AllNotesOff()
	{
		for(int note = 0; note < NUM_MIDI_NOTES; ++note)
		{
			NoteOff(note, 64);
		}
	}

	void MidiChannel::Write(PCM16* data, int count)
	{
		for(int note = 0; note < NUM_MIDI_NOTES; ++note)
		{
			notes[note].Write(data, count);
		}
	}
}
