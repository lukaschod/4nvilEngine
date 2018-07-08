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
#if ENABLED_WINDOWS
#include <Core/Tools/Windows/Common.hpp>
#include <Core/Tools/IO/Directory.hpp>

using namespace Core;

const Directory& Directory::GetExecutablePath()
{
    static Directory path;
    static Bool pathValid = false;

    // Store executable path on demand
    if (!pathValid)
    {
        // Get the actual directory
        auto hModule = GetModuleHandleW(NULL);
        GetModuleFileName(hModule, path.data, (DWORD) path.GetCapacity());

        // Add terminator
        auto last = strrchr(path.data, '\\');
        ASSERT(last != nullptr);
        *(++last) = 0;
        pathValid = true;
    }

    ASSERT(pathValid);

    return path;
}
#endif