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
#include <ctime>

namespace Core
{
    // Represents an instant in time, typically expressed as a date and time of day
    // TODO: Finish it
    struct DateTime
    {
    public:
        inline DateTime() : ticks(0) {}
        inline DateTime(UInt64 ticks) : ticks(ticks) {}

        inline Bool operator==(const DateTime& rhs) const { return ticks == rhs.ticks; }
        inline Bool operator!=(const DateTime& rhs) const { return ticks != rhs.ticks; }
        inline Bool operator>(const DateTime& rhs) const { return ticks > rhs.ticks; }
        inline Bool operator<(const DateTime& rhs) const { return ticks < rhs.ticks; }

        inline UInt32 GetYear() const { return GetTmStructure().tm_year; }
        inline UInt32 GetMonth() const { return GetTmStructure().tm_mon; }
        inline UInt32 GetDay() const { return GetTmStructure().tm_mday; }
        inline UInt32 GetHour() const { return GetTmStructure().tm_hour; }
        inline UInt32 GetMinute() const { return GetTmStructure().tm_min; }
        inline UInt32 GetSecond() const { return GetTmStructure().tm_sec; }
        inline UInt64 GetTicks() const { return ticks; }

        // Gets a DateTime object that is set to the current date and time on this computer, expressed as the local time
        inline static DateTime Now()
        {
            auto ticks = std::time(nullptr);
            return DateTime(ticks);
        }

    private:
        inline tm GetTmStructure() const
        {
            tm localTime;
            CHECK(localtime_s(&localTime, &ticks) == 0);
            return localTime;
        }

    private:
        std::time_t ticks;
    };
}