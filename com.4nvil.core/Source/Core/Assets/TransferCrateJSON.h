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

        virtual Void Transfer(Void* data, UInt size) override
        {
            WriteLevel();
            WriteExpected("\"value\": ");
            base::Transfer(data, size);
            WriteExpected(",\n");
        }

        virtual Void TransferPointer(Transferable*& transferable) override
        {
            WriteLevel();
            level++;
            WriteExpected("{\n");
            base::TransferPointer(transferable);
            WriteExpected("},\n");
            level--;
        }

        virtual Bool IsWritting() const override { return false; }
        virtual Char* GetName() const override { return "json.1"; }

        virtual Void Reset(IO::Stream* stream) override
        {
            base::Reset(stream);
            level = 0;
        }

        Void WriteLevel()
        {
            for (UInt16 i = 0; i < level; i++)
                WriteExpected("    ");
        }

        Void WriteExpected(const Char* value)
        {
            base::Transfer((Void*) value, strlen(value));
        }

    private:
        UInt16 level;
    };

    class TransferCrateJSONReader : public TransferCrateBinaryReader
    {
    public:
        BASE_IS(TransferCrateBinaryReader);

        virtual Void Transfer(Void* data, UInt size) override
        {
            ReadLevel();
            ReadExpected("\"value\": ");
            base::Transfer(data, size);
            ReadExpected(",\n");
        }

        virtual Void TransferPointer(Transferable*& transferable) override
        {
            ReadLevel();
            level++;
            ReadExpected("{\n");
            base::TransferPointer(transferable);
            ReadExpected("},\n");
            level--;
        }

        virtual Bool IsReading() const override { return true; }
        virtual Char* GetName() const override { return "json.1"; }

        virtual Void Reset(IO::Stream* stream, CrateDependancySolver* dependancySolver, CrateModule* crateModule) override
        {
            base::Reset(stream, dependancySolver, crateModule);
        }

        Void ReadLevel()
        {
            for (UInt16 i = 0; i < level; i++)
                ReadExpected("    ");
        }

        Void ReadExpected(const Char* value)
        {
            Char temp[20];
            auto size = strlen(value);
            base::Transfer(temp, size);
            ASSERT(memcmp(temp, value, size) == 0); // Check if it is expected text
        }

    private:
        UInt16 level;
    };
}