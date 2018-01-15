#pragma once

#include <stdio.h>
#include <cstdarg>

#if PLATFORM_WINDOWS
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
#	endif

#	ifndef NOMINMAX
#		define NOMINMAX
#	endif

#	include <windows.h>
#endif

namespace Console
{
	inline void Write(const char* msg) 
	{ 
#if PLATFORM_WINDOWS
		OutputDebugString(msg);
#else
		ERROR("Not implemented");
#endif
	}

	inline void WriteFmt(const char* format, ...)
	{
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		Write(buffer);
	}
};