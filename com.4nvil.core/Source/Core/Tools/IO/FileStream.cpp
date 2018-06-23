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

#include <Core/Tools/IO/FileStream.hpp>
#include <stdarg.h>

using namespace Core::IO;

FileStream::FileStream()
    : isOpened(false)
{
}

FileStream::~FileStream()
{
    ASSERT(!isOpened);
}

Bool FileStream::Open(const Char* path, FileMode mode, FileAccess access)
{
    ASSERT_MSG(file == nullptr, "File is already opened");
    auto modeTexted = TryGetMode(mode, access);
    file = fopen(path, modeTexted);
    isOpened = file != nullptr;
    return isOpened;
}

Void FileStream::Close()
{
    ASSERT(isOpened);
    fclose(file);
    isOpened = false;
}

Void FileStream::Read(Void* data, UInt size)
{
    ASSERT(isOpened);
    fread(data, sizeof(UInt8), size, file);
}

Void FileStream::Write(Void* data, UInt size)
{
    ASSERT(isOpened);
    fwrite(data, sizeof(UInt8), size, file);
}

Void FileStream::ReadFmt(const Char* format, ...)
{
    ASSERT(isOpened);
    va_list ap;
    va_start(ap, format);
    vfscanf(file, format, ap);
    va_end(ap);
}

Void FileStream::WriteFmt(const Char* format, ...)
{
    ASSERT(isOpened);
    va_list ap;
    va_start(ap, format);
    vfprintf(file, format, ap);
    va_end(ap);
}

Void FileStream::WriteFmt(const Char* format, va_list arguments)
{
    ASSERT(isOpened);
    vfprintf(file, format, arguments);
}

Void FileStream::SetPosition(UInt position)
{
    fseek(file, position, SEEK_SET);
}

UInt FileStream::GetPosition() const
{
    return ftell(file);
}

Void FileStream::Flush()
{
    ASSERT(isOpened);
    fflush(file);
}

const Char* FileStream::TryGetMode(FileMode mode, FileAccess access)
{
    switch (mode)
    {

    case FileMode::Append:
    {
        switch (access)
        {
        case FileAccess::Read:
            return "rab+";
        case FileAccess::Write:
            return "wab+";
        default:
            return nullptr;
        }
    }

    case FileMode::Open:
    {
        switch (access)
        {
        case FileAccess::Read:
            return "rb";
        case FileAccess::Write:
            return "wb";
        case FileAccess::ReadWrite:
            return "rb+";
        default:
            return nullptr;
        }
    }

    case FileMode::Create:
    {
        switch (access)
        {
        case FileAccess::Read:
            return "rb+";
        case FileAccess::Write:
            return "wb";
        case FileAccess::ReadWrite:
            return "wb+";
        default:
            return nullptr;
        }
    }

    }
    return nullptr;
}
