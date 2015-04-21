#ifndef AUDIO_TYPES_H
#define AUDIO_TYPES_H

#include <SDL/SDL.h>

#define NUM_MIDI_CHANNELS 16
#define NUM_MIDI_NOTES 128

typedef signed short PCM16;

namespace Audio
{
	enum MidiMessages
	{
		MIDI_NOTE_OFF = 8,
		MIDI_NOTE_ON = 9,
		MIDI_AFTERTOUCH,
		MIDI_CONTROL_CHANGE,
		MIDI_PROGRAM_CHANGE,
		MIDI_CHANNEL_PRESSURE,
		MIDI_PITCH_WHEEL,
		MIDI_SYSTEM
	};

	enum MidiSystemMessages
	{
		MIDI_SEQUENCE_NUM =	0x00,
		MIDI_TEXT =			0x01,
		MIDI_COPYRIGHT =	0x02,
		MIDI_TRACK_NAME =	0x03,
		MIDI_INSTRUMENT =	0x04,
		MIDI_LYRIC =		0x05,
		MIDI_MARKER =		0x06,
		MIDI_CUE_POINT =	0x07,
		MIDI_PROGRAM_NAME =	0x08,
		MIDI_DEVICE_NAME =	0x09,
		MIDI_CHANNEL_PRE =	0x20,
		MIDI_PORT =			0x21,
		MIDI_END_OF_TRACK =	0x2F,
		MIDI_SET_TEMPO =	0x51,
		MIDI_SMTPE_OFFSET =	0x54,
		MIDI_TIME_SIG =		0x58,
		MIDI_KEY_SIG =		0x59,
		MIDI_SYSEX_META =	0x7f
	};

	enum MidiControlMessages
	{
		MIDI_BANK_SELECT = 0,
		MIDI_CHANNEL_VOLUME = 7,
		MIDI_ALL_NOTES_OFF = 123,
	};

	struct MidiEvent
	{
		MidiEvent(): time(0), eventCode(0), param1(0), param2(0) {}
		Uint32 time;
		Uint8 eventCode;
		Uint8 systemMessage;
		Uint32 param1;
		Uint32 param2;
	};
}
#endif