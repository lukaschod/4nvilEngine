#pragma once

#include <Core\Tools\Windows\Common.h>
#include <Core\Tools\Common.h>
#include <Core\Tools\IO\Directory.h>

namespace Core::Directory
{
	const char* GetExecutablePath()
	{
		static char path[maxPathSize];
		static bool pathValid = false;

		// Store executable path on demand
#if ENABLED_WINDOWS
		if (!pathValid)
		{
			HMODULE hModule = GetModuleHandleW(NULL);
			GetModuleFileName(hModule, path, maxPathSize);
			char* last = strrchr(path, '\\');
			*last = 0;
			pathValid = true;
		}
#endif

		ASSERT(pathValid);

		return path;
	}
}