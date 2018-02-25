#include <stdio.h>
#include <cstdarg>
#include <Tools\Windows\Common.h>
#include <Tools\Console.h>

namespace Core::Console
{
	void Write(const char* msg)
	{
#if ENABLED_WINDOWS
		OutputDebugString(msg);
#else
		ERROR("Not implemented");
#endif
	}

	void WriteFmt(const char* format, ...)
	{
		char buffer[1024];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 1024, format, ap);
		Write(buffer);
	}
};