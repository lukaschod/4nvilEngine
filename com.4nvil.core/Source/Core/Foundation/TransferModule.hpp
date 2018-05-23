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

namespace Core
{
    class ITransfer
    {
    public:
        virtual Void Transfer(UInt8* data, UInt size) = 0;
        virtual Bool IsReading() const = 0;
    };

    class TransferBinaryReader : public ITransfer
    {
    public:
        TransferBinaryReader(IO::Stream* stream) : stream(stream) {}

        virtual Void Transfer(UInt8* data, UInt size) override { stream->Read(data, size); }
        virtual Bool IsReading() const override { return true; }

    private:
        IO::Stream* stream;
    };

    class TransferBinaryWritter : public ITransfer
    {
    public:
        TransferBinaryWritter(IO::Stream* stream) : stream(stream) {}

        virtual Void Transfer(UInt8* data, UInt size) override { stream->Write(data, size); }
        virtual Bool IsReading() const override { return false; }

    private:
        IO::Stream* stream;
    };

    struct Transferable
    {
        virtual Void Transfer(ITransfer* transfer) = 0;
        Guid guid;
    };

    class TransferModule : public ComputeModule
    {
    public:
        Void Read(IO::Stream* stream, Transferable* value)
        {
            TransferBinaryReader transfer(stream);
            value->Transfer(&transfer);
        }

        Void Write(IO::Stream* stream, Transferable* value)
        {
            TransferBinaryWritter transfer(stream);
            value->Transfer(&transfer);
        }
    };
}