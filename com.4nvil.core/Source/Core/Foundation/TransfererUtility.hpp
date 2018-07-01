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
#include <Core/Foundation/TransfererModule.hpp>

namespace Core
{
    template<> inline Void TransferValue(ITransfer* transfer, Guid& value) { transfer->Transfer((UInt8*) &value, sizeof(Guid)); }
    template<> inline Void TransferValue(ITransfer* transfer, Directory& value) { transfer->Transfer((UInt8*) &value, sizeof(Directory)); }

    template<class T> inline Void TransferValue(ITransfer* transfer, List<T>& value)
    {
        auto size = value.size();
        transfer->Transfer((UInt8*) &size, sizeof(UInt));

        // Reserve data
        // TODO: We can push optimization with removed constructor
        if (transfer->IsReading())
            value.resize(size);

        if (size != 0)
            transfer->Transfer((UInt8*) value.data(), sizeof(T) * size);
    }

    template<> inline Void TransferValue(ITransfer* transfer, List<Transferable*>& value)
    {
        auto size = value.size();
        transfer->Transfer((UInt8*) &size, sizeof(UInt));

        // Reserve data
        // TODO: We can push optimization with removed constructor
        if (transfer->IsReading())
            value.resize(size);

        for (UInt i = 0; i < size; i++)
            TransferValue(transfer, value[i]);
    }
}

namespace Core
{
    // Finds the total size of transfered data
    // Note: It does not write or read anything
    class TransferFindSize : public ITransfer
    {
    public:
        TransferFindSize() : size(0) {}
        virtual Void Transfer(UInt8* data, UInt size) override
        {
            this->size += size;
        }
        virtual Void TransferPointer(Transferable*& transferable)
        {
            transferable->Transfer(this);
        }

    public:
        UInt size;
    };

    // Simply binary deserializer
    class TransferBinaryReader : public ITransfer
    {
    public:
        TransferBinaryReader(IO::Stream* stream) : stream(stream) {}

        virtual Void Transfer(UInt8* data, UInt size) override { stream->Read(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override { transferable->Transfer(this); }
        virtual Bool IsReading() const override { return true; }

    private:
        IO::Stream* stream;
    };

    // Simply binary serializer
    class TransferBinaryWritter : public ITransfer
    {
    public:
        TransferBinaryWritter(IO::Stream* stream) : stream(stream) {}

        virtual Void Transfer(UInt8* data, UInt size) override { stream->Write(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override { transferable->Transfer(this); }
        virtual Bool IsWritting() const override { return false; }

    private:
        IO::Stream* stream;
    };

    class TransferJSONReader : public ITransfer
    {
    public:
        TransferJSONReader(IO::Stream* stream) : stream(stream), level(0) {}

        virtual Void Transfer(UInt8* data, UInt size) override 
        { 
            ReadLevel();
            Read("\"value\": ");
            stream->Read(data, size);
            Read(",\n");
        }

        virtual Void TransferPointer(Transferable*& transferable) override 
        { 
            ReadLevel();
            level++;
            Read("{\n");
            transferable->Transfer(this);
            Read("},\n");
            level--;
        }

        virtual Bool IsReading() const override { return true; }

        Void ReadLevel()
        {
            for (UInt16 i = 0; i < level; i++)
                Read("    ");
        }

        Void Read(const Char* value)
        {
            Char temp[20];
            auto size = strlen(value);
            stream->Read(temp, size);
            ASSERT(memcmp(temp, value, size) == 0); // Check if it is expected text
        }

    private:
        IO::Stream* stream;
        UInt16 level;
    };

    class TransferJSONWritter : public ITransfer
    {
    public:
        TransferJSONWritter(IO::Stream* stream) : stream(stream), level(0) {}

        virtual Void Transfer(UInt8* data, UInt size) override 
        {
            WriteLevel();
            Write("\"value\": ");
            stream->Write(data, size);
            Write(",\n");
        }

        virtual Void TransferPointer(Transferable*& transferable) override 
        {
            WriteLevel();
            level++;
            Write("{\n");
            transferable->Transfer(this);
            Write("},\n");
            level--;
        }

        virtual Bool IsWritting() const override { return false; }

        Void WriteLevel()
        {
            for (UInt16 i = 0; i < level; i++)
                Write("    ");
        }

        Void Write(const Char* value)
        {
            stream->Write(value, strlen(value));
        }

    private:
        IO::Stream* stream;
        UInt16 level;
    };
}