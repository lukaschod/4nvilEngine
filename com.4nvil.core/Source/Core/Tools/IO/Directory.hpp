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
#include <Core/Tools/DateTime.hpp>
#include <Core/Tools/Character.hpp>
#include <Core/Tools/Collections/List.hpp>

namespace Core
{
    class DirectoryExtension
    {
    public:
        inline DirectoryExtension() { data = nullptr; }
        inline DirectoryExtension(const Char8* extension) { data = extension; }

        inline Bool operator==(const DirectoryExtension& lhs) const { return Character::Equals(data, lhs.data); }
        inline Bool operator!=(const DirectoryExtension& lhs) const { return Character::NotEquals(data, lhs.data); }

    private:
        const Char* data;
    };

    class Directory 
    {
    public:
        inline Directory() { *data = 0; size = 0; }
        CORE_API Directory(const Directory& directory);
        CORE_API Directory(const Char8* path);

        // Append value on end of directory
        CORE_API Bool Append(const Char8* value);

        // Is directory a file, checks not extension
        CORE_API Bool IsFile() const;

        // Returns extension of directory
        CORE_API Bool GetExtension(DirectoryExtension& extension) const;

        CORE_API Bool GetWriteTime(DateTime& date) const;
        CORE_API Bool SetWriteTime(const DateTime& date) const;

        inline UInt GetCapacity() const { return 260; }

        inline UInt GetSize() const { return size; }

        inline const Char* ToCString() const { return data; }

        inline Bool operator==(const Directory& lhs) const { return Character::Equals(data, lhs.data); }
        inline Bool operator!=(const Directory& lhs) const { return Character::NotEquals(data, lhs.data); }

        inline Void RecalculateSize() { size = Character::Length(data); }

        // Returns path to current executable location
        CORE_API static const Directory& GetExecutablePath();

        // Returns the names of subdirectories (including their paths) in the specified directory
        CORE_API static Void GetDirectories(const Directory& path, List<Directory>& out);

        // Returns the names of files (including their paths) in the specified directory
        CORE_API static Void GetFiles(const Directory& path, List<Directory>& out);

    private:
        inline Directory(const wchar_t* directory);

    private:
        Char8 data[260];
        UInt size;
    };
}