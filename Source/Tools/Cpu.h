#pragma once

#include <Tools\Common.h>

#if _WIN32
#include <Windows.h>
#endif

namespace Cpu
{
	uint64_t GetCycles()
	{
		FILETIME ftime, fsys, fuser;
		GetProcessTimes(GetCurrentProcess(), &ftime, &ftime, &fsys, &fuser);
		return fuser.dwLowDateTime;
	}

	uint32_t GetCoreCount()
	{
		return 4; // TODO:
	}
}