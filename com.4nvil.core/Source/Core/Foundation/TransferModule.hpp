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
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Tools/IO/Stream.hpp>
#include <Core/Foundation/PipeModule.hpp>

#define IMPLEMENT_TRANSFERABLE(NameSpace, Name) \
    virtual TransfererId& GetTransfererId() const override { static TransfererId id(#NameSpace "::" #Name); return id; } \
    virtual Void Transfer(ITransfer* transfer) override;

#define IMPLEMENT_TRANSFERER(NameSpace, Name) \
    virtual const Transferable* AllocateTransferable() const override { return Allocate##Name(); } \
    virtual Void RecCreateTransferable(const ExecutionContext& context, const Transferable* target) override { RecCreate##Name(context, (const Name*) target); } \
    virtual Void RecDestroyTransferable(const ExecutionContext& context, const Transferable* target) override { RecDestroy(context, (const Name*) target); } \
    virtual TransfererId& GetTransfererId() const override { static TransfererId id(#NameSpace "::" #Name); return id; }

namespace Core
{
    struct Transferable;

    class ITransfer
    {
    public:
        virtual Void Transfer(UInt8* data, UInt size) pure;
        virtual Void TransferPointer(Transferable*& transferable) pure;
        virtual Bool IsReading() const pure;
    };

    struct TransferableId
    {
        Bool operator==(const TransferableId& rhs) const { return directory == rhs.directory; }
        Bool operator!=(const TransferableId& rhs) const { return directory != rhs.directory; }

        Directory directory;
    };

    struct TransfererId
    {
        TransfererId() { *data = 0; }
        TransfererId(const Char* name) { strcpy(data, name); }
        Bool operator==(const TransfererId& rhs) const { return strcmp(data, rhs.data) == 0; }
        Bool operator!=(const TransfererId& rhs) const { return strcmp(data, rhs.data) != 0; }

        Char data[20];
    };

    struct Transferable
    {
        TransferableId* id = nullptr;
        virtual TransfererId& GetTransfererId() const pure;
        virtual Void Transfer(ITransfer* transfer) pure;
    };

    class TransfererModule : public PipeModule
    {
    public:
        virtual const Transferable* AllocateTransferable() const pure;
        virtual Void RecCreateTransferable(const ExecutionContext& context, const Transferable* target) pure;
        virtual Void RecDestroyTransferable(const ExecutionContext& context, const Transferable* target) pure;
        virtual TransfererId& GetTransfererId() const pure;
    };
}

// TODO: Move it somewhere
namespace Core
{
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
        virtual Bool IsReading() const override { return true; }

    public:
        UInt size;
    };

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

    class TransferBinaryWritter : public ITransfer
    {
    public:
        TransferBinaryWritter(IO::Stream* stream) : stream(stream) {}

        virtual Void Transfer(UInt8* data, UInt size) override { stream->Write(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override { transferable->Transfer(this); }
        virtual Bool IsReading() const override { return false; }

    private:
        IO::Stream* stream;
    };
}