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

#include <Core/Tools/Windows/Common.hpp>
#include <Core/Tools/Common.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <filesystem>

namespace Core::Directory
{
    namespace filesystem = std::experimental::filesystem;

    const char* GetExecutablePath()
    {
        static char path[maxPathSize];
        static bool pathValid = false;

        // Store executable path on demand
#if ENABLED_WINDOWS
        if (!pathValid)
        {
            HMODULE hModule = GetModuleHandleW(NULL);
            GetModuleFileName(hModule, path, maxPathSize);
            char* last = strrchr(path, '\\');
            *last = 0;
            pathValid = true;
        }
#endif

        ASSERT(pathValid);

        return path;
    }

    List<String> GetDirectories(String& path)
    {
        List<String> out;
        for (auto& directory : filesystem::directory_iterator(path))
            if (directory.status().type() == filesystem::v1::file_type::directory)
                out.push_back(directory.path().string());
        return out;
    }

    List<String> GetFiles(String& path)
    {
        List<String> out;
        for (auto& directory : filesystem::directory_iterator(path))
            if (directory.status().type() == filesystem::v1::file_type::regular)
                out.push_back(directory.path().string());
        return out;
    }
}