#pragma once

namespace Core::Console
{
	// Prints message into the IDEA console
	void Write(const char* msg);

	// Prints formated message into the IDEA console
	void WriteFmt(const char* format, ...);
};