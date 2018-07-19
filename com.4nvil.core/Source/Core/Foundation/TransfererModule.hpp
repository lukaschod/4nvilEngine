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

#define TRANSFER(Value) TransferValue(transfer, Value);

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
        virtual Void Transfer(const Char* format, ...) {}
        virtual Void Transfer(Void* data, UInt size) pure;
        virtual Void TransferPointer(Transferable*& transferable) pure;
        virtual Bool IsReading() const { return false; }
        virtual Bool IsWritting() const { return false; }
    };

    template<class T> inline Void TransferValue(ITransfer* transfer, T& value) { transfer->Transfer(&value, sizeof(value)); }
    template<class T> inline Void TransferValue(ITransfer* transfer, T*& value) { transfer->TransferPointer((Transferable*&) value); }

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