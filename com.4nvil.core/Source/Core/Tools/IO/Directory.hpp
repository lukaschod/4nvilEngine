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

#include <Core/Tools/String.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <wchar.h>

namespace Core
{
    class Directory 
    {
    public:
        Directory() { *data = 0; }
        Directory(const wchar_t* directory) { wcscpy(data, directory); }
        Directory(const Directory& directory) { wcscpy(data, directory.data); }

        Bool IsFile() const;
        Bool GetExtension(DirectoryExtension& extension) const;

        inline UInt GetCapacity() const { return 260; }

        inline UInt GetSize() const { return size; }

        inline const wchar_t* ToCString() const { return data; }
        inline wchar_t* ToString() { return data; }

        inline Bool operator==(const Directory& lhs) const { return wcscmp(data, lhs.data) == 0; }
        inline Bool operator!=(const Directory& lhs) const { return wcscmp(data, lhs.data) != 0; }

        // Returns path to current executable location
        static const Directory& GetExecutablePath();

        // Returns the names of subdirectories (including their paths) in the specified directory
        static Void GetDirectories(const Directory& path, List<Directory>& out);

        // Returns the names of files (including their paths) in the specified directory
        static Void GetFiles(const Directory& path, List<Directory>& out);

    private:
        wchar_t data[260];
        UInt size;
    };

    class DirectoryExtension
    {
    public:
        DirectoryExtension() { data = nullptr; }
        DirectoryExtension(const wchar_t* extension) { data = extension; }

        Bool operator==(const DirectoryExtension& lhs) const { return wcscmp(data, lhs.data) == 0; }
        Bool operator!=(const DirectoryExtension& lhs) const { return wcscmp(data, lhs.data) != 0; }

    private:
        const wchar_t* data;
    };
}