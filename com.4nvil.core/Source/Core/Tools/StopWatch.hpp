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

#include <Core/Tools/Common.hpp>
#include <chrono>

namespace Core
{
    class StopWatch
    {
    private:
        typedef std::chrono::high_resolution_clock clock;
        typedef std::chrono::duration<UInt64, std::pico> picoseconds;
        typedef std::chrono::duration<UInt64, std::milli> milliseconds;
        typedef std::chrono::duration<UInt64, std::micro> microseconds;
        typedef std::chrono::duration<Float64, typename clock::period> Cycle;

    public:
        StopWatch()
            : isRunning(false)
        {
        }

        inline Void Start()
        {
            ASSERT(!isRunning);
            start = end = clock::now();
            isRunning = true;
        }

        inline Void Restart()
        {
            start = end = clock::now();
            isRunning = true;
        }

        inline Void Stop()
        {
            ASSERT(isRunning);
            end = clock::now();
            isRunning = false;
        }

        inline UInt64 GetElapsedPicoseconds() const
        {
            auto current = isRunning ? clock::now() : clock::now();
            auto ticks_per_iter = Cycle(current - start) / 1;
            return std::chrono::duration_cast<picoseconds>(ticks_per_iter).count();
        }

        inline UInt64 GetElapsedMicroseconds() const
        {
            auto current = isRunning ? clock::now() : clock::now();
            auto ticks_per_iter = Cycle(current - start) / 1;
            return std::chrono::duration_cast<microseconds>(ticks_per_iter).count();
        }

        inline UInt64 GetElapsedMiliseconds() const
        {
            auto current = isRunning ? clock::now() : clock::now();
            auto ticks_per_iter = Cycle(current - start) / 1;
            return std::chrono::duration_cast<milliseconds>(ticks_per_iter).count();
        }

    private:
        std::chrono::high_resolution_clock::time_point start, end;
        Bool isRunning;
    };
}