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

#include <stdlib.h>
#include <stdio.h>
#include <cstdarg>

namespace Core
{
    template<UInt Size>
    class StringBuilder
    {
    public:
        StringBuilder()
            : current(text)
            , end(text + Size)
        {
        }

        inline Void Append(Void* data, UInt size)
        {
            ASSERT(current < end);
            memcpy(current, data, size);
            current += size;
        }

        inline Void Append(const Char* value)
        {
            ASSERT(current < end);
            auto size = strlen(value);
            memcpy(current, value, size);
            current += size;
        }

        inline Void AppendFmt(const Char* format, ...)
        {
            ASSERT(current < end);
            va_list ap;
            va_start(ap, format);
            current += vsnprintf(current, Size, format, ap);
            va_end(ap);
        }

        inline Void AppendFmt(const Char* format, va_list ap)
        {
            ASSERT(current < end);
            current += vsnprintf(current, Size, format, ap);
        }

        inline const Char* ToString()
        {
            *current = 0; // Add null terminator on demand
            return text;
        }

        inline UInt GetSize()
        {
            return (UInt) (current - text) + 1;
        }

    private:
        Char text[Size];
        Char* end;
        Char* current;
    };
}
