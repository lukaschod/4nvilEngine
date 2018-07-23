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

#include <Core/Assets/TransferCrateBinary.h>

namespace Core
{
    class TransferCrateJSONWritter : public TransferCrateBinaryWritter
    {
    public:
        BASE_IS(TransferCrateBinaryWritter);

        virtual Void Transfer(const Char* name, Transferable* value) override
        {
            Level();
            Expected(name);
            Expected(":\n");
            level++;
            value->Transfer(this);
            level--;
        }

        virtual Void Transfer(const Char* name, Guid* value) override
        {
            Char guidText[37];
            value->ToString(guidText);

            Level();
            Expected(name);
            Expected(": ");
            stream->Write(guidText, 36);
            Expected("\n");
        }

        virtual Void Transfer(const Char* name, UInt64* value) override
        {
            Level();
            Expected(name);
            Expected(": ");
            stream->WriteFmt("%lld", *value);
            Expected("\n");
        }

        virtual Void Transfer(const Char* name, UInt32* value) override
        {
            Level();
            Expected(name);
            Expected(": ");
            stream->WriteFmt("%ld", *value);
            Expected("\n");
        }

        virtual Void Transfer(const Char* name, Char* value) override
        {
            Level();
            Expected(name);
            UInt size = Character::Length(value);
            stream->WriteFmt(": [%lld]\"", size);
            stream->Write(value, size);
            Expected("\"\n");
        }

        virtual Void Transfer(const Char* name, Void* data, UInt size) override
        {
            Level();
            Expected(name);
            Expected(": ");
            stream->Write(data, size);
            Expected("\n");
        }

        virtual Void TransferPointer(const Char* name, Transferable*& transferable) override
        {
            Level();
            level++;
            Expected(name);
            Expected(":\n");
            base::TransferPointer(name, transferable);
            level--;
        }

        virtual Bool IsWritting() const override { return false; }
        virtual Char* GetName() const override { return "json.1"; }

        virtual Void Reset(IO::Stream* stream) override
        {
            base::Reset(stream);
            level = 0;
        }

        /*virtual Void WriteLocalResource(Crate* crate, UInt localIndex) override
        {
            WriteExpected(crate->locals[localIndex].transfererId.data);
            WriteExpected(": \n");
            level++;
            base::WriteLocalResource(crate, localIndex);
            level--;
        }*/

        Void Level()
        {
            for (UInt16 i = 0; i < level; i++)
                Expected("    ");
        }

        Void Expected(const Char* value)
        {
            stream->Write((Void*) value, Character::Length(value));
        }

    private:
        UInt16 level;
    };

    class TransferCrateJSONReader : public TransferCrateBinaryReader
    {
    public:
        BASE_IS(TransferCrateBinaryReader);

        virtual Void Transfer(const Char* name, Transferable* value) override
        {
            Level();
            Expected(name);
            Expected(":\n");
            level++;
            value->Transfer(this);
            level--;
        }

        virtual Void Transfer(const Char* name, Guid* value) override
        {
            Char guidText[37];

            Level();
            Expected(name);
            Expected(": ");
            stream->Read(guidText, 36);
            Expected("\n");

            CHECK(value->TryParse(guidText, *value));
        }

        virtual Void Transfer(const Char* name, UInt64* value) override
        {
            Level();
            Expected(name);
            Expected(": ");
            stream->ReadFmt("%lld", value);
            Expected("\n");
        }

        virtual Void Transfer(const Char* name, UInt32* value) override
        {
            Level();
            Expected(name);
            Expected(": ");
            stream->ReadFmt("%ld", value);
            Expected("\n");
        }

        virtual Void Transfer(const Char* name, Char* value) override
        {
            UInt size;
            Expected(name);
            stream->ReadFmt(": [%lld]\"", &size);
            stream->Read(value, size);
            Expected("\"\n");
        }

        virtual Void Transfer(const Char* name, Void* data, UInt size) override
        {
            Level();
            Expected(name);
            Expected(": ");
            stream->Read(data, size);
            Expected("\n");
        }

        virtual Void TransferPointer(const Char* name, Transferable*& transferable) override
        {
            Level();
            level++;
            Expected(name);
            Expected(":\n");
            base::TransferPointer(name, transferable);
            level--;
        }

        virtual Bool IsReading() const override { return true; }
        virtual Char* GetName() const override { return "json.1"; }

        virtual Void Reset(IO::Stream* stream, CrateDependancySolver* dependancySolver, CrateModule* crateModule) override
        {
            base::Reset(stream, dependancySolver, crateModule);
        }

        /*virtual Void ReadLocalResource(const ExecutionContext& context, Crate* crate, UInt localIndex) override
        {
            Expected(crate->locals[localIndex].transfererId.data);
            Expected(": \n");
            level++;
            base::ReadLocalResource(context, crate, localIndex);
            level--;
        }*/

        Void Level()
        {
            for (UInt16 i = 0; i < level; i++)
                Expected("    ");
        }

        Void Expected(const Char* value)
        {
            Char temp[128];
            auto size = Character::Length(value);
            stream->Read(temp, size);
            ASSERT(memcmp(temp, value, size) == 0); // Check if it is expected text
        }

    private:
        UInt16 level;
    };
}