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

#include <stdio.h>
#include <cstdarg>
#include <Core/Tools/Windows/Common.hpp>
#include <Core/Tools/Console.hpp>

namespace Core::Console
{
    Void Write(const Char* msg)
    {
#if ENABLED_WINDOWS
        OutputDebugString(msg);
#else
        ERROR("Not implemented");
#endif
    }

    Void WriteFmt(const Char* format, ...)
    {
        Char buffer[1024];
        va_list ap;
        va_start(ap, format);
        vsprintf_s(buffer, 1024, format, ap);
        Write(buffer);
    }
};