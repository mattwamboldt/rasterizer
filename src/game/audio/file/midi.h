#ifndef AUDIO_FILE_MIDI
#define AUDIO_FILE_MIDI

#include "../audio.h"
#include <vector>
#include <string>

namespace Audio
{
	class MidiTrack
	{
	public:
		MidiEvent Current(){ return events[currentEvent]; }
		MidiEvent Advance(){ return events[++currentEvent]; }
		
		bool Load(SDL_RWops* file);
	private:
		Uint32 ReadLength(SDL_RWops* file);
		Uint8 NextByte(SDL_RWops* file);
		std::string MidiTrack::ReadString(SDL_RWops* file, Uint32 length);
		bool IsCommandByte(){ return currentByte & 128;} //The first bit is 1 on a command byte

		Uint32 numBytes;
		Uint32 numBytesRemaining;
		Uint8 currentByte;
		Uint16 sequenceNumber;

		int currentEvent;
		std::vector<MidiEvent> events;
		std::string name;
		std::string copyright;
		std::string text;
		std::string instrument;
	};

	class MidiFile
	{
	public:
		bool Load(const char* path);
		void Write(PCM16* data, int count);
	private:
		std::vector<MidiTrack> tracks;
		int bpm; // Beats Per Minute
		int ppqn; // Pulses Per Quater Note ( a quarter note is a beat )
		Uint16 format; //Format 0 is a one track file, 1 is multitrack simul and 2 is multisong
		Uint16 numTracks;
	};
}

#endif
