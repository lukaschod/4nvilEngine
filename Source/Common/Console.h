#pragma once

#if _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <stdio.h>
#include <cstdarg>
#include <windows.h>

namespace Console
{
	inline void Print(const char* msg) 
	{ 
		OutputDebugString(msg);
	}

	inline void Printf(const char* format, ...)
	{
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		Print(buffer);
	}
};
#else
namespace Console
{
	inline void Print(const char* msg) {}
	inline void Printf(const char* format, ...) {}
};
#endif