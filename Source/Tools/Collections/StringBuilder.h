#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <cstdarg>

template<size_t Size>
class StringBuilder
{
public:
	StringBuilder()
		: current(text)
		, end(text + Size)
	{
	}

	inline void Append(void* data, size_t size)
	{
		ASSERT(current < end);
		memcpy(current, data, size);
		current += size;
	}

	inline void Append(const char* value)
	{
		ASSERT(current < end);
		auto size = strlen(value);
		memcpy(current, value, size);
		current += size;
	}

	inline void AppendFmt(const char* format, ...)
	{
		ASSERT(current < end);
		va_list ap;
		va_start(ap, format);
		current += vsnprintf(current, Size, format, ap);
		va_end(ap);
	}

	inline void AppendFmt(const char* format, va_list ap)
	{
		ASSERT(current < end);
		current += vsnprintf(current, Size, format, ap);
	}

	inline const char* ToString()
	{
		*current = 0; // Add null terminator on demand
		return text;
	}

	inline size_t GetSize()
	{
		return (size_t)(current - text) + 1;
	}

private:
	char text[Size];
	char* end;
	char* current;
};
