#pragma once

#include <Tools\Common.h>
#include <chrono>

namespace Core
{
	class StopWatch
	{
	private:
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<unsigned long long, std::pico> picoseconds;
		typedef std::chrono::duration<unsigned long long, std::milli> milliseconds;
		typedef std::chrono::duration<unsigned long long, std::micro> microseconds;
		typedef std::chrono::duration<double, typename clock::period> Cycle;

	public:
		StopWatch()
			: isRunning(false)
		{
		}

		inline void Start()
		{
			ASSERT(!isRunning);
			start = end = clock::now();
			isRunning = true;
		}

		inline void Restart()
		{
			start = end = clock::now();
			isRunning = true;
		}

		inline void Stop()
		{
			ASSERT(isRunning);
			end = clock::now();
			isRunning = false;
		}

		inline uint64_t GetElapsedPicoseconds() const
		{
			auto current = isRunning ? clock::now() : clock::now();
			auto ticks_per_iter = Cycle(current - start) / 1;
			return std::chrono::duration_cast<picoseconds>(ticks_per_iter).count();
		}

		inline uint64_t GetElapsedMicroseconds() const
		{
			auto current = isRunning ? clock::now() : clock::now();
			auto ticks_per_iter = Cycle(current - start) / 1;
			return std::chrono::duration_cast<microseconds>(ticks_per_iter).count();
		}

		inline uint64_t GetElapsedMiliseconds() const
		{
			auto current = isRunning ? clock::now() : clock::now();
			auto ticks_per_iter = Cycle(current - start) / 1;
			return std::chrono::duration_cast<milliseconds>(ticks_per_iter).count();
		}

	private:
		std::chrono::high_resolution_clock::time_point start, end;
		bool isRunning;
	};
}