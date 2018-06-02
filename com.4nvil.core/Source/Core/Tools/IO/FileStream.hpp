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
        FileStream();
        ~FileStream();

        virtual Void Close() override;
        virtual Void Read(Void* data, UInt size) override;
        virtual Void ReadFmt(const char* format, ...) override;
        virtual Void Write(Void* data, UInt size) override;
        virtual Void WriteFmt(const char* format, ...) override;
        virtual Void WriteFmt(const char* format, va_list arguments) override;

        Bool Open(const char* path, FileMode mode, FileAccess access);
        inline Bool IsOpened() const { return isOpened; }

        // Forces all unwritten data to be written to the file
        Void Flush();

    private:
        const char* TryGetMode(FileMode mode, FileAccess access);

    private:
        FILE* file;
        Bool isOpened;
    };
}