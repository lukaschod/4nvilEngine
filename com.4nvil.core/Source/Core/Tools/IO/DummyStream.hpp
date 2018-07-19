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

#include <Core/Tools/IO/Stream.hpp>

namespace Core::IO
{
    class DummyStream : public Stream
    {
    public:
        virtual Void Close() override {}
        virtual Void Read(Void* data, UInt size) override { position += size; }
        virtual Void Write(const Void* data, UInt size) override { position += size; }
        virtual Void SetPosition(UInt position) override { this->position = position; }
        virtual UInt GetPosition() const override { return position; }
    private:
        UInt position;
    };
}