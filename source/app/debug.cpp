#include "debug.h"
#include <cstdio>
#include <windows.h>

namespace Debug
{
	//TODO: Make this a circular flushing buffer so we can write larger messages
	char buffer[4096];

	//Writes out the given message to the log file
	void log(int category, const char* fmt, ...)
	{
		va_list argptr;
		va_start(argptr,fmt);
		vsnprintf(buffer, 4096, fmt, argptr);
		SDL_LogDebug(category, buffer);
	}

	void log(const char* fmt, ...)
	{
		va_list argptr;
		va_start(argptr,fmt);
		vsnprintf(buffer, 4096, fmt, argptr);
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, buffer);
	}

	//TODO: Set up categories append to the string when using the console
	//Writes out the given message to the debug console
	void console(int category, const char* fmt, ...)
	{
		va_list argptr;
		va_start(argptr,fmt);
		vsnprintf(buffer, 4096, fmt, argptr);
		OutputDebugStringA(buffer);
	}

	void console(const char* fmt, ...)
	{
		va_list argptr;
		va_start(argptr,fmt);
		vsnprintf(buffer, 4096, fmt, argptr);
		OutputDebugStringA(buffer);
	}
}