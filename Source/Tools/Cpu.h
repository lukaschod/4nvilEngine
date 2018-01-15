#pragma once

#include <Tools\Common.h>

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Cpu
{
	uint64_t GetCycles()
	{
#if PLATFORM_WINDOWS
		FILETIME ftime, fsys, fuser;
		GetProcessTimes(GetCurrentProcess(), &ftime, &ftime, &fsys, &fuser);
		return fuser.dwLowDateTime;
#else
		ERROR("Not implemented");
#endif
	}

	uint32_t GetCoreCount()
	{
		return 4; // TODO:
	}
}