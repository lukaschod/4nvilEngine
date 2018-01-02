#pragma once

#include <Tools\Common.h>
#if _WIN32
#include <Windows.h>
#endif

namespace Directory
{
	const char* GetExecutablePath()
	{
		static char path[MAX_PATH];
		static bool pathValid = false;

#if _WIN32
		if (!pathValid)
		{
			HMODULE hModule = GetModuleHandleW(NULL);
			GetModuleFileName(hModule, path, MAX_PATH);
			char* last = strrchr(path, '\\');
			*last = 0;
			pathValid = true;
		}
#endif

		ASSERT(pathValid);

		return path;
	}
}