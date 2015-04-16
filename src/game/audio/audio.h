#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL.h>

typedef signed short PCM16;

namespace Audio
{
	//these settings are used for all of the audio components
	extern SDL_AudioSpec audioSpec;

	bool Init();
}

#endif