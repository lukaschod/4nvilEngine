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
        virtual Void Close() pure;
        virtual Void Read(Void* data, UInt size) pure;
        virtual Void ReadFmt(const Char* format, ...) { NOT_IMPLEMENTED(); };
        virtual Void ReadFmt(const Char* format, va_list arguments) { NOT_IMPLEMENTED(); };
        virtual Void Write(const Void* data, UInt size) pure;
        virtual Void WriteFmt(const Char* format, ...) { NOT_IMPLEMENTED(); };
        virtual Void WriteFmt(const Char* format, va_list arguments) { NOT_IMPLEMENTED(); };
        virtual Void SetPosition(UInt position) { NOT_IMPLEMENTED(); }
        virtual UInt GetPosition() const { return 0; NOT_IMPLEMENTED();
        }
    };
}