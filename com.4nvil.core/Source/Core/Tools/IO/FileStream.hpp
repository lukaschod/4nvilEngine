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
#include <Core/Tools/IO/Stream.hpp>
#include <stdio.h>

namespace Core::IO
{
    enum class FileMode
    {
        Append,
        Open,
        Create,
    };

    enum class FileAccess
    {
        Read,
        ReadWrite,
        Write,
    };

    class FileStream : public Stream
    {
    public:
        CORE_API FileStream();
        CORE_API ~FileStream();

        CORE_API virtual Void Close() override;
        CORE_API virtual Void Read(Void* data, UInt size) override;
        CORE_API virtual Void ReadString(Char* value, UInt capacity) override;
        CORE_API virtual Void ReadFmt(const Char* format, ...) override;
        CORE_API virtual Void Write(const Void* data, UInt size) override;
        CORE_API virtual Void WriteString(const Char* value) override;
        CORE_API virtual Void Write(const Char* format, va_list arguments) override;
        CORE_API virtual Void WriteFmt(const Char* format, ...) override;
        CORE_API virtual Void SetPosition(UInt position) override;
        CORE_API virtual UInt GetPosition() const override;

        CORE_API Bool Open(const Char* path, FileMode mode, FileAccess access);

        inline Bool IsOpened() const { return isOpened; }

        // Forces all unwritten data to be written to the file
        CORE_API Void Flush();

    private:
        const Char* TryGetMode(FileMode mode, FileAccess access);

    private:
        FILE* file;
        Bool isOpened;
    };
}