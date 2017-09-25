#pragma once

#include <Common\EngineCommon.h>
#include <chrono>

class StopWatch
{
private:
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<unsigned long long, std::pico> picoseconds;
	typedef std::chrono::duration<unsigned long long, std::milli> milliseconds;
	typedef std::chrono::duration<double, typename clock::period> Cycle;

public:
	inline void Start()
	{
		start = end = clock::now();
	}

	inline void Stop()
	{
		end = clock::now();
	}

	inline uint64_t GetElapsedPicoseconds()
	{
		auto ticks_per_iter = Cycle(end - start) / 1;
		return std::chrono::duration_cast<picoseconds>(ticks_per_iter).count();
	}

	inline uint64_t GetElapsedMiliseconds()
	{
		auto ticks_per_iter = Cycle(end - start) / 1;
		return std::chrono::duration_cast<milliseconds>(ticks_per_iter).count();
	}

private:
	std::chrono::high_resolution_clock::time_point start, end;
};