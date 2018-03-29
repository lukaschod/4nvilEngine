/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core\Tools\Common.hpp>
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

		inline uint64 GetElapsedPicoseconds() const
		{
			auto current = isRunning ? clock::now() : clock::now();
			auto ticks_per_iter = Cycle(current - start) / 1;
			return std::chrono::duration_cast<picoseconds>(ticks_per_iter).count();
		}

		inline uint64 GetElapsedMicroseconds() const
		{
			auto current = isRunning ? clock::now() : clock::now();
			auto ticks_per_iter = Cycle(current - start) / 1;
			return std::chrono::duration_cast<microseconds>(ticks_per_iter).count();
		}

		inline uint64 GetElapsedMiliseconds() const
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