#include "midi.h"
#include "../../debug.h"
#include <cstring>

namespace Audio
{
	Uint8 MidiTrack::NextByte(SDL_RWops* file)
	{
		currentByte = SDL_ReadU8( file );
		--numBytesRemaining;
		return currentByte;
	}

	Uint32 MidiTrack::ReadLength(SDL_RWops* file)
	{
		Uint32 length = 0;
		while(NextByte( file ) & 128)
		{
			length += (currentByte - 128);
			length = length << 7;
		}

		length += currentByte;
		return length;
	}

	std::string MidiTrack::ReadString(SDL_RWops* file, Uint32 length)
	{
		char* temp = new char[length];
		SDL_RWread(file, temp, 1, length);
		numBytesRemaining -= length;

		std::string result = std::string(temp, length);
		delete[] temp;
		
		return result;
	}

	bool MidiTrack::Load(SDL_RWops* file)
	{
		currentEvent = 0;
		pulseCounter = 0;

		char chunkID[4];
		SDL_RWread( file, chunkID, 4, 1);
		if(strncmp(chunkID, "MTrk", 4))
		{
			Debug::console("MIDI PARSER: invalid track format\n");
			return false;
		}

		numBytes = numBytesRemaining = SDL_ReadBE32( file );

		//Reading an individual midi event
		MidiEvent ev;
		Uint8 command = 0;

		bool reading = true;
		while(numBytesRemaining > 0)
		{
			ev.param1 = 0;
			ev.param2 = 0;

			//We start with a variable length time code
			ev.time = ReadLength(file);

			//Then we read the event status
			Uint32 param1 = NextByte(file);

			if(IsCommandByte())
			{
				command = currentByte >> 4;

				//we need to check for system events
				if(command == MIDI_SYSTEM)
				{
					//Meta events
					if(currentByte == 0xFF)
					{
						Uint8 metaType = NextByte(file);
						Uint32 paramlength = ReadLength(file);
						Uint32 newTempo = 0;
						Uint8 port = 0;
						Uint8 channelPrefix = 0;

						ev.eventCode = 0xFF;
						ev.systemMessage = metaType;

						switch(metaType)
						{
						case MIDI_SEQUENCE_NUM:
							ev.param1 = SDL_ReadBE16(file);
							numBytesRemaining -= paramlength;
							//Debug::console( "MIDI Parser: Sequence Num Event %u\n", sequenceNumber);
							break;

						case MIDI_TEXT:
							text = ReadString(file, paramlength);
							//Debug::console( "MIDI Parser: Text Event %s\n", text.c_str() );
							break;

						case MIDI_COPYRIGHT:
							copyright = ReadString(file, paramlength);
							//Debug::console( "MIDI Parser: Copyright Event %s\n", name.c_str() );
							break;

						case MIDI_TRACK_NAME:
							name = ReadString(file, paramlength);
							//Debug::console( "MIDI Parser: Track Name Event %s\n", name.c_str() );
							break;

						case MIDI_INSTRUMENT:
							instrument = ReadString(file, paramlength);
							//Debug::console( "MIDI Parser: Instrument Event %s\n", instrument.c_str() );
							break;

						case MIDI_LYRIC:
							//Debug::console( "MIDI Parser: Lyric Event\n" );
							SDL_RWseek(file, paramlength, RW_SEEK_CUR);
							numBytesRemaining -= paramlength;
							break;

						case MIDI_MARKER:
							//Debug::console( "MIDI Parser: Marker Event\n" );
							SDL_RWseek(file, paramlength, RW_SEEK_CUR);
							numBytesRemaining -= paramlength;
							break;

						case MIDI_CUE_POINT:
							//Debug::console( "MIDI Parser: Cure Point Event\n" );
							SDL_RWseek(file, paramlength, RW_SEEK_CUR);
							numBytesRemaining -= paramlength;
							break;

						case MIDI_CHANNEL_PRE:
							ev.param1 = NextByte(file);
							//Debug::console( "MIDI Parser: Channel Prefix Event %u\n", channelPrefix );
							break;

						case MIDI_PORT:
							ev.param1 = NextByte(file);
							//Debug::console( "MIDI Parser: Port Event %u\n", port );
							break;

						case MIDI_END_OF_TRACK:
							//Debug::console( "MIDI Parser: End of Track Event\n" );
							return true;
							break;

						case MIDI_SET_TEMPO:
							//We need to read in the 24 bit number from big endian format
							newTempo = NextByte(file);
							newTempo <<= 8;
							newTempo += NextByte(file);
							newTempo <<= 8;
							newTempo += NextByte(file);
							ev.param1 = newTempo;
							//Debug::console( "MIDI Parser: Set Tempo Event %u\n", newTempo );
							break;

						case MIDI_SMTPE_OFFSET:
							//Debug::console( "MIDI Parser: SMPTE Offset Event\n" );
							NextByte(file);
							NextByte(file);
							NextByte(file);
							NextByte(file);
							NextByte(file);
							break;

						case MIDI_TIME_SIG:
							//Debug::console( "MIDI Parser: Time Signature Event\n" );
							NextByte(file);
							NextByte(file);
							NextByte(file);
							NextByte(file);
							break;

						case MIDI_KEY_SIG:
							//Debug::console( "MIDI Parser: Key Signature Event\n" );
							NextByte(file);
							NextByte(file);
							break;

						case MIDI_SYSEX_META:
							SDL_RWseek(file, paramlength, RW_SEEK_CUR);
							numBytesRemaining -= paramlength;
							//Debug::console( "MIDI Parser: Sequencer-Specific Meta Event\n" );
							break;

						default:
							//Debug::console( "MIDI Parser: Unrecognized event type\n" );
							SDL_RWseek(file, paramlength, RW_SEEK_CUR);
							numBytesRemaining -= paramlength;
							break;
						}

						events.push_back(ev);
					}
					//Sysex events
					else
					{
						Uint32 sysexLength = ReadLength(file);
						
						//We skip over these since they don't mean anything to us at the moment
						SDL_RWseek(file, sysexLength, RW_SEEK_CUR);
						numBytesRemaining -= sysexLength;
					}

					continue;
				}
				else
				{
					ev.eventCode = currentByte;
					param1 = NextByte(file);
				}
			}

			ev.param1 = param1;

			//Then we check for parameters
			switch(command)
			{
			case MIDI_NOTE_OFF:
			case MIDI_NOTE_ON:
			case MIDI_AFTERTOUCH:
			case MIDI_CONTROL_CHANGE:
			case MIDI_PITCH_WHEEL:
			case MIDI_SYSTEM:
				ev.param2 = NextByte(file);
				break;
			}

			/*Uint8 channel = ev.eventCode & 0x0F;

			switch(command)
			{
			case MIDI_NOTE_OFF:
				Debug::console( "MIDI Parser: Note Off Event - Channel %u, Note %u, Velocity %u\n",
					channel, ev.param1, ev.param2 );
				break;
			case MIDI_NOTE_ON:
				Debug::console( "MIDI Parser: Note On Event - Channel %u, Note %u, Velocity %u\n",
					channel, ev.param1, ev.param2 );
				break;
			case MIDI_AFTERTOUCH:
				Debug::console( "MIDI Parser: Aftertouch Event - Channel %u, Note %u, Pressure %u\n",
					channel, ev.param1, ev.param2 );
				break;
			case MIDI_CONTROL_CHANGE:
				Debug::console( "MIDI Parser: Control Change Event - Channel %u, Controller %u, Value %u\n",
					channel, ev.param1, ev.param2 );
				break;
			case MIDI_PITCH_WHEEL:
				Debug::console( "MIDI Parser: Pitch Bend Event - Channel %u, LSB %u, MSB %u\n",
					channel, ev.param1, ev.param2 );
				break;

			case MIDI_PROGRAM_CHANGE:
				Debug::console( "MIDI Parser: Program Change Event - Channel %u, Program %u\n",
					channel, ev.param1 );
				break;
			case MIDI_CHANNEL_PRESSURE:
				Debug::console( "MIDI Parser: Channel Pressure Event - Channel %u, Pressure %u\n",
					channel, ev.param1 );
				break;
			}*/

			events.push_back(ev);
		}

		SDL_RWseek(file, numBytesRemaining, RW_SEEK_CUR);
		return true;
	}

	bool MidiFile::Load(const char* path)
	{
		tracks.clear();
		SDL_RWops* file = SDL_RWFromFile( path, "r+b" );
		//File does not exist
		if( file == NULL )
		{
			Debug::console( "Warning: Unable to open file! SDL Error: %s\n", SDL_GetError() );
	        return false;
		}
		else
		{
			//Midi files are in big endian format and to be multiplatform means we can't do any fancy memory mapping stuff.
			char chunkID[4];

			SDL_RWread( file, chunkID, 4, 1);
			if(strncmp(chunkID, "MThd", 4))
			{
				Debug::console("MIDI PARSER: invalid header format\n");
				return false;
			}

			//We ignore the size here
			Uint32 chunkSize = SDL_ReadBE32( file );

			//Format description
			format = SDL_ReadBE16( file );

			if(format > 1)
			{
				Debug::console("MIDI PARSER: Cannot yet handle Type 2 midi files.\n");
				return false;
			}

			numTracks = SDL_ReadBE16( file );
			Uint16 timing = SDL_ReadBE16( file );

			if(timing & (1 << 15))
			{
				//metric timing (uses fps) which we'll need to convert to samples per second
				Debug::console("MIDI PARSER: Can't handle SMTPE timing yet.\n");
				return false;
			}
			else
			{
				//time units per quarter note or beat
				pulsesPerBeat = timing;
			}

			for(int i = 0; i < numTracks; ++i)
			{
				MidiTrack nextTrack;
				nextTrack.Load(file);
				tracks.push_back(nextTrack);
			}

			SDL_RWclose( file );
			return true;
		}
	}

	void MidiTrack::Advance(MidiController* controller)
	{
		++pulseCounter;
		while(currentEvent < events.size() && pulseCounter >= Current().time)
		{
			controller->ProcessEvent(Current());
			++currentEvent;
			pulseCounter = 0;
		}
	}

	void MidiFile::Advance(MidiController* controller)
	{
		for(int i = 0; i < numTracks; ++i)
		{
			tracks[i].Advance(controller);
		}
	}
}
