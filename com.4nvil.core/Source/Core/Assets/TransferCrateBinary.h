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

#include <Core/Tools/IO/FileStream.hpp>
#include <Core/Foundation/TransfererModule.hpp>
#include <Core/Assets/CrateSolvers.h>

namespace Core
{
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, ResourceDependancy& value)
    {
        TRANSFER(value.type);
        TRANSFER(value.index);
    }

    class TransferCrateBinaryWritter : public ITransfer
    {
    public:
        BASE_IS(ITransfer);

        virtual Void Transfer(const Char* name, Transferable* value) override { value->Transfer(this); }
        virtual Void Transfer(const Char* name, Guid* value) override { stream->Write(value, sizeof(value)); }
        virtual Void Transfer(const Char* name, UInt64* value) override { stream->Write(value, sizeof(value)); }
        virtual Void Transfer(const Char* name, UInt32* value) override { stream->Write(value, sizeof(value)); }
        virtual Void Transfer(const Char* name, Char* value) override
        { 
            UInt size = Character::Length(value) + 1;
            stream->Write(&size, sizeof(size));
            stream->Write(value, size);
        }
        virtual Void Transfer(const Char* name, Void* data, UInt size) override { stream->Write(data, size); }
        virtual Void TransferPointer(const Char* name, Transferable*& transferable) override
        {
            auto& resource = currentResource->dependencies[currentDependancyIndex++];
            TransferValue(this, name, resource);
        }
        virtual Bool IsWritting() const override { return true; }
        virtual Char* GetName() const { return "binary.1"; }

        virtual Void Reset(IO::Stream* stream)
        {
            this->stream = stream;
        }

        virtual Void WriteLocalResource(Crate* crate, UInt localIndex)
        {
            this->stream = stream;
            currentResource = &crate->locals[localIndex];
            currentResource->offset = stream->GetPosition();
            currentDependancyIndex = 0;

            auto transferable = const_cast<Transferable*>(currentResource->cachedTransferable);
            ASSERT(transferable != nullptr);
            Transfer(transferable->GetTransfererId().data, transferable);
        }

    protected:
        IO::Stream* stream;
        ResourceLocal* currentResource;
        UInt currentDependancyIndex;
    };

    class TransferCrateBinaryReader : public ITransfer
    {
    public:
        BASE_IS(ITransfer);

        virtual Void Transfer(const Char* name, Transferable* value) override { value->Transfer(this); }
        virtual Void Transfer(const Char* name, Guid* value) override { stream->Read(value, sizeof(value)); }
        virtual Void Transfer(const Char* name, UInt64* value) override { stream->Read(value, sizeof(value)); }
        virtual Void Transfer(const Char* name, UInt32* value) override { stream->Read(value, sizeof(value)); }
        virtual Void Transfer(const Char* name, Char* value) override
        {
            UInt size;
            stream->Read(&size, sizeof(size));
            stream->Read(value, size);
        }
        virtual Void Transfer(const Char* name, Void* data, UInt size) override { stream->Read(data, size); }
        virtual Void TransferPointer(const Char* name, Transferable*& transferable) override
        {
            ResourceDependancy resource;
            TransferValue(this, name, resource);

            // Store dependancy, we will resolve it later on, once all the transferables are loaded
            dependancySolver->Add(cachedCrate, (const Transferable*&) transferable, resource);
        }
        virtual Bool IsReading() const override { return true; }
        virtual Char* GetName() const { return "binary.1"; }

        virtual Void Reset(IO::Stream* stream, CrateDependancySolver* dependancySolver, CrateModule* crateModule)
        {
            this->stream = stream;
            this->dependancySolver = dependancySolver;
            this->crateModule = crateModule;
        }

        virtual Void ReadLocalResource(const ExecutionContext& context, Crate* crate, UInt localIndex)
        {
            cachedCrate = crate;
            auto& resource = cachedCrate->locals[localIndex];

            // Allocate transferable
            auto transferer = const_cast<TransfererModule*>(crateModule->FindTransferer(resource.transfererId));
            ASSERT(transferer != nullptr);
            auto transferable = const_cast<Transferable*>(transferer->AllocateTransferable());

            // Cache it
            resource.cachedTransferable = transferable;

            // Transfer its data
            stream->SetPosition(resource.offset);
            Transfer(transferable->GetTransfererId().data, transferable);

            // Issue the create request
            transferer->RecCreateTransferable(context, transferable);
        }

    protected:
        CrateModule* crateModule;
        CrateDependancySolver* dependancySolver;
        IO::Stream* stream;
        Crate* cachedCrate;
    };
}