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
#include <filesystem>

using namespace Core;
namespace filesystem = std::experimental::filesystem;

Directory::Directory(const Directory& directory)
{ 
    strcpy(data, directory.data); 
    RecalculateSize(); 
}

Directory::Directory(const Char8* path)
{
    strcpy(data, path);
    RecalculateSize();
}

Bool Directory::Append(const Char8* value)
{
    // Check if it will not exceed the capacity
    auto sizeOfValue = Character::Length(value);
    if (GetSize() + sizeOfValue > GetCapacity())
        return false;

    strcpy(data + size, value);
    RecalculateSize();
    return true;
}

Bool Directory::IsFile() const
{
    return filesystem::is_regular_file(data);
}

Bool Directory::GetExtension(DirectoryExtension& extension) const
{
    auto last = strrchr(data, '.');
    if (last == nullptr)
        return false;

    extension = last;
    return true;
}

Bool Directory::GetWriteTime(DateTime& date) const
{
    std::error_code error;
    auto time = filesystem::last_write_time(data, error);
    auto cftime = filesystem::file_time_type::clock::to_time_t(time);
    date = DateTime(cftime);
    return (Bool)error == false;
}

Bool Directory::SetWriteTime(const DateTime& date) const
{
    auto time = filesystem::file_time_type::clock::from_time_t(date.GetTicks());
    std::error_code error;
    filesystem::last_write_time(data, time, error);
    return (Bool)error == false;
}

Void Directory::GetDirectories(const Directory& path, List<Directory>& out)
{
    for (auto& directory : filesystem::directory_iterator(path.ToCString()))
    {
        //if (directory.status().type() == filesystem::v1::file_type::directory)
        {
            auto utf8 = directory.path().u8string();
            Directory directory(utf8.c_str());
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
            auto utf8 = directory.path().u8string();
            Directory directory(utf8.c_str());
            out.push_back(directory);
        }
    }
}
