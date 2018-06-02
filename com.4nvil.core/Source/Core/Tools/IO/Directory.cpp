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
#include <Core/Tools/Common.hpp>
#include <Core/Tools/IO/Directory.hpp>
#if ENABLED_WINDOWS
#   include <Core/Tools/Windows/Common.hpp>
#endif
#include <filesystem>

using namespace Core;
namespace filesystem = std::experimental::filesystem;

Directory::Directory(const Directory& directory)
{ 
    strcpy(data, directory.data); 
    RecalculateSize(); 
}

Directory::Directory(const wchar_t* directory)
{
    wcstombs(data, directory, 260);
    RecalculateSize();
}

Bool Directory::Append(const Char8* value)
{
    // Check if it will not exceed the capacity
    auto sizeOfValue = strlen(value);
    if (GetSize() + sizeOfValue > GetCapacity())
        return false;

    strcpy(data + size, value);
    RecalculateSize();
    return true;
}

Bool Directory::IsFile() const
{
    filesystem::directory_entry temp(data);
    return temp.status().type() == filesystem::v1::file_type::regular;
}

Bool Directory::GetExtension(DirectoryExtension& extension) const
{
    auto last = strrchr(data, '.');
    if (last == nullptr)
        return false;

    extension = last;
    return true;
}

const Directory& Directory::GetExecutablePath()
{
    static Directory path;
    static Bool pathValid = false;

    // Store executable path on demand
#if ENABLED_WINDOWS
    if (!pathValid)
    {
        // Get the actual directory
        auto hModule = GetModuleHandleW(NULL);
        GetModuleFileName(hModule, path.data, (DWORD) path.GetCapacity());

        // Add terminator
        auto last = strrchr(path.data, '\\');
        *last = 0;
        pathValid = true;
    }
#endif

    ASSERT(pathValid);

    return path;
}

Void Directory::GetDirectories(const Directory& path, List<Directory>& out)
{
    for (auto& directory : filesystem::directory_iterator(path.ToCString()))
    {
        //if (directory.status().type() == filesystem::v1::file_type::directory)
        {
            Directory directory(directory.path().c_str());
            out.push_back(directory);
        }
    }
}

Void Directory::GetFiles(const Directory& path, List<Directory>& out)
{
    for (auto& directory : filesystem::directory_iterator(path.ToCString()))
    {
        if (directory.status().type() == filesystem::v1::file_type::regular)
        {
            Directory directory(directory.path().c_str());
            out.push_back(directory);
        }
    }
}

Void Directory::RecalculateSize() { size = strlen(data); }
