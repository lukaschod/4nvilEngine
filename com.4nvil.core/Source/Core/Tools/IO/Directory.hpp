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
#include <Core/Tools/String.hpp>
#include <Core/Tools/Collections/List.hpp>

namespace Core
{
    class DirectoryExtension
    {
    public:
        DirectoryExtension() { data = nullptr; }
        DirectoryExtension(const Char8* extension) { data = extension; }

        Bool operator==(const DirectoryExtension& lhs) const { return strcmp(data, lhs.data) == 0; }
        Bool operator!=(const DirectoryExtension& lhs) const { return strcmp(data, lhs.data) != 0; }

    private:
        const Char* data;
    };

    class Directory 
    {
    public:
        Directory() { *data = 0; size = 0; }
        Directory(const Directory& directory);

        // Append value on end of directory
        Bool Append(const Char8* value);

        // Is directory a file, checks not extension
        Bool IsFile() const;

        // Returns extension of directory
        Bool GetExtension(DirectoryExtension& extension) const;

        inline UInt GetCapacity() const { return 260; }

        inline UInt GetSize() const { return size; }

        inline const char* ToCString() const { return data; }

        inline Bool operator==(const Directory& lhs) const { return strcmp(data, lhs.data) == 0; }
        inline Bool operator!=(const Directory& lhs) const { return strcmp(data, lhs.data) != 0; }

        // Returns path to current executable location
        static const Directory& GetExecutablePath();

        // Returns the names of subdirectories (including their paths) in the specified directory
        static Void GetDirectories(const Directory& path, List<Directory>& out);

        // Returns the names of files (including their paths) in the specified directory
        static Void GetFiles(const Directory& path, List<Directory>& out);

    private:
        inline Directory(const wchar_t* directory);
        inline Void RecalculateSize();

    private:
        Char8 data[260];
        UInt size;
    };
}