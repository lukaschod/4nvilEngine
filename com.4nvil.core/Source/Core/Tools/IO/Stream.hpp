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

#include <stdio.h>
#include <Core/Tools/Common.hpp>

namespace Core::IO
{
    class Stream
    {
    public:
        virtual Void Close() = 0;
        virtual Void Read(Void* data, UInt size) = 0;
        virtual Void ReadFmt(const char* format, ...) { NOT_IMPLEMENTED(); };
        virtual Void ReadFmt(const char* format, va_list arguments) { NOT_IMPLEMENTED(); };
        virtual Void Write(Void* data, UInt size) = 0;
        virtual Void WriteFmt(const char* format, ...) { NOT_IMPLEMENTED(); };
        virtual Void WriteFmt(const char* format, va_list arguments) { NOT_IMPLEMENTED(); };
    };
}