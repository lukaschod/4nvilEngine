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
#include <Core/Tools/Guid.hpp>
#include <Core/Tools/IO/Stream.hpp>
#include <Core/Foundation/ComputeModule.hpp>

#define TRANSFER(Veriable, ITransfer) ITransfer->Transfer(##Veriable, typeof(Veriable), (UInt8*)&Veriable, sizeof(Veriable));

namespace Core
{
    class ITransfer
    {
    public:
        virtual Void Transfer(const char* name, const type_info& type, UInt8* data, UInt size) = 0;
    };

    struct Serializable
    {
        Guid guid;

        virtual Void Transfer(const ITransfer* transferer) = 0;
    };

    Void Transfer(const char* name, const ITransfer* transferer, UInt32 value)
    {

    }

    class TransferModule : public ComputeModule
    {
    public:
        Void Read(IO::Stream* stream, Serializable* value)
        {
            value->Transfer(nullptr);
        }
    };
}