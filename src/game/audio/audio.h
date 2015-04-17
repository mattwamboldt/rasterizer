#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL.h>
#include "types.h"
#include "synth/midicontroller.h"

namespace Audio
{
	//these settings are used for all of the audio components
	extern SDL_AudioSpec audioSpec;
	extern MidiController midiController;

	bool Init();

	//TODO: Write a tool that converts source audio files to a proprietary base raw format for faster reads
}

#endif