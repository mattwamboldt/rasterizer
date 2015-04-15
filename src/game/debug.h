#ifndef DEBUG_H
#define DEBUG_H

#include <SDL/SDL.h>

namespace Debug
{
	enum Category
	{
		APPLICATION = SDL_LOG_CATEGORY_APPLICATION, //application
		ERROR = SDL_LOG_CATEGORY_ERROR, //error log
		SYSTEM = SDL_LOG_CATEGORY_SYSTEM, //system log
		AUDIO = SDL_LOG_CATEGORY_AUDIO, //audio log
		VIDEO = SDL_LOG_CATEGORY_VIDEO, //video log
		RENDER = SDL_LOG_CATEGORY_RENDER, //render log
		INPUT = SDL_LOG_CATEGORY_INPUT // input log
	};

	//Writes out the given message to the log file
	void log(int category, const char* fmt, ...);
	void log(const char* fmt, ...);

	//Writes out the given message to the debug console
	void console(int category, const char* fmt, ...);
	void console(const char* fmt, ...);
}

#endif