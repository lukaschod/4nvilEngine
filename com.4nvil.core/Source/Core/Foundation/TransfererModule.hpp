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
#include <Core/Tools/Character.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Tools/Guid.hpp>
#include <Core/Tools/IO/Stream.hpp>
#include <Core/Foundation/PipeModule.hpp>

#define TRANSFER(Value) TransferValue(transfer, #Value, Value);

// Special macro used for automatic declarations of transfering structure
// Note: Add it inside the structure
#define IMPLEMENT_TRANSFERABLE(NameSpace, Name) \
    virtual TransfererId& GetTransfererId() const override { static TransfererId id(#NameSpace "::" #Name); return id; } \
    virtual Void Transfer(ITransfer* transfer) override;

// Special mecro use for automatic implementations of transfering methods for module
// Note: Add it inside the class
#define IMPLEMENT_TRANSFERER(NameSpace, Name) \
    virtual const Transferable* AllocateTransferable() override { return Allocate##Name(); } \
    virtual Void RecCreateTransferable(const ExecutionContext& context, const Transferable* target) override { RecCreate##Name(context, (const Name*) target); } \
    virtual Void RecDestroyTransferable(const ExecutionContext& context, const Transferable* target) override { RecDestroy(context, (const Name*) target); } \
    virtual TransfererId& GetTransfererId() const override { static TransfererId id(#NameSpace "::" #Name); return id; } \
    virtual const Char* GetName() const override { return #NameSpace "::" #Name; }

namespace Core
{
    struct Transferable;

    class ITransfer
    {
    public:
        virtual Void Transfer(const Char* name, Transferable* value) {}
        virtual Void Transfer(const Char* name, Guid* value) {}
        virtual Void Transfer(const Char* name, UInt64* value) {}
        virtual Void Transfer(const Char* name, UInt32* value) {}
        virtual Void Transfer(const Char* name, Char* value) {}
        virtual Void Transfer(const Char* name, Void* data, UInt size) {}
        virtual Void TransferPointer(const Char* name, Transferable*& transferable) pure;
        virtual Bool IsReading() const { return false; }
        virtual Bool IsWritting() const { return false; }
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
        Bool operator==(const TransfererId& rhs) const { return Character::Equals(data, rhs.data); }
        Bool operator!=(const TransfererId& rhs) const { return Character::NotEquals(data, rhs.data); }
        UInt GetCapacity() const { return 20; }

        Char data[20];
    };

    struct Transferable
    {
        virtual TransfererId& GetTransfererId() const pure;
        virtual Void Transfer(ITransfer* transfer) pure;
        TransferableId* id = nullptr;
    };

    class TransfererModule : public PipeModule
    {
    public:
        virtual const Transferable* AllocateTransferable() pure;
        virtual Void RecCreateTransferable(const ExecutionContext& context, const Transferable* target) pure;
        virtual Void RecDestroyTransferable(const ExecutionContext& context, const Transferable* target) pure;
        virtual TransfererId& GetTransfererId() const pure;
    };
}

namespace Core
{
    template<class T> inline Void TransferValue(ITransfer* transfer, const Char* name, T& value) { transfer->Transfer(name, &value, sizeof(value)); }
    template<class T> inline Void TransferValue(ITransfer* transfer, const Char* name, T*& value) { transfer->TransferPointer(name, (Transferable*&) value); }

    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, UInt64& value) { transfer->Transfer(name, &value); }
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, UInt32& value) { transfer->Transfer(name, &value); }
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, Guid& value) { transfer->Transfer(name, &value); }
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, Directory& value)
    {
        transfer->Transfer(name, (Char*) value.ToCString());

        if (transfer->IsReading())
            value.RecalculateSize();
    }
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, TransferableId& value) { TransferValue(transfer, name, value.directory); }
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, TransfererId& value) { transfer->Transfer(name, value.data); }

    template<class T> inline Void TransferValue(ITransfer* transfer, const Char* name, List<T>& value)
    {
        auto size = value.size();
        TransferValue(transfer, "size", size);

        // Reserve data
        // TODO: We can push optimization with removed constructor
        if (transfer->IsReading())
            value.resize(size);

        for (UInt i = 0; i < size; i++)
            TransferValue(transfer, "item", value[i]);
    }
}