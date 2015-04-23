#include "midicontroller.h"
#include "../file/midi.h"
#include "../../debug.h"

namespace Audio
{
	void MidiController::RefreshPulseRate()
	{
		if(midiFile)
		{
			// convert microseconds to seconds, multiply by samples per second and divide by pulses per beat
			// eg. 500000 us/beat = 0.5 s/beat * 44100 samples/s = 22100 samples/beat
			// 22100 samples/beat / 96 pulses/beat = 230.2 samples/pulse
			timePerPulse = (timePerBeat / 1000000.0f) / midiFile->pulsesPerBeat;
		}
	}

	void MidiController::Init(MidiFile* file)
	{
		MidiNote::SetScale();
		timePerSample = 1.0 / audioSpec.freq;
		timePerBeat = 500000; //initially 120 bpm
		timeSincePulse = 0.0f;
		midiFile = file;
		RefreshPulseRate();

		if(midiFile)
		{
			midiFile->Advance(this);
		}

		for(int i = 0; i < NUM_MIDI_CHANNELS; ++i)
		{
			channels[i].Init();
		}
	}

	void MidiController::ProcessEvent(const MidiEvent& e)
	{
		int command = e.eventCode >> 4;
		int channel = e.eventCode & 0x0F;

		switch(command)
		{
		case MIDI_NOTE_ON:
			channels[channel].NoteOn(e.param1, e.param2);
			break;

		case MIDI_NOTE_OFF:
			channels[channel].NoteOff(e.param1, e.param2);
			break;
		
		case MIDI_CONTROL_CHANGE:
			ControlChange(channel, e.param1, e.param2);
			break;

		case MIDI_SYSTEM:
			SystemEvent(e.systemMessage, e.param1, e.param2);
			break;

		default:
			Debug::console("MIDI Controller: Command not yet Implemented %u\n", command);
		}
	}

	void MidiController::SystemEvent(Uint8 systemMessage, Uint32 p1, Uint32 p2)
	{
		switch(systemMessage)
		{
		case MIDI_SET_TEMPO:
			timePerBeat = p1;
			RefreshPulseRate();
			break;
		default:
			Debug::console("MIDI Controller: System Message not yet implemented %u\n", systemMessage);
		}
	}

	void MidiController::ControlChange(Uint8 channel, Uint8 controller, Uint8 value)
	{
		switch(controller)
		{
		case MIDI_CHANNEL_VOLUME:
			channels[channel].SetVolume(value / 127.0);
			break;

		case MIDI_ALL_NOTES_OFF:
			channels[channel].AllNotesOff();
			break;

		default:
			Debug::console("MIDI Controller: Control Change not yet implemented %u\n", controller);
		}
	}

	void MidiController::Write(PCM16* data, int count)
	{
		Uint32 samplesWritten = 0;
		Uint32 numSamplesToWrite = (timePerPulse - timeSincePulse) / timePerSample;
		if(!midiFile)
		{
			numSamplesToWrite = count;
		}

		while(samplesWritten < count && numSamplesToWrite + samplesWritten < count)
		{
			for(int i = 0; i < NUM_MIDI_CHANNELS; ++i)
			{
				channels[i].Write(data + samplesWritten, numSamplesToWrite);
			}

			samplesWritten += numSamplesToWrite;
			timeSincePulse = timePerSample * numSamplesToWrite - timePerPulse;

			if(midiFile)
			{
				midiFile->Advance(this);
			}

			numSamplesToWrite = (timePerPulse - timeSincePulse) / timePerSample;
		}

		if(samplesWritten < count)
		{
			for(int i = 0; i < NUM_MIDI_CHANNELS; ++i)
			{
				channels[i].Write(data + samplesWritten, count - samplesWritten);
			}

			timeSincePulse += timePerSample * (count - samplesWritten);
		}
	}
}