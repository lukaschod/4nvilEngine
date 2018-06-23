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

#include <Core/Tools/IO/FileStream.hpp>
#include <Core/Assets/CrateModule.hpp>
#include <Core/Foundation/TransferModule.hpp>

using namespace Core;
using namespace Core::IO;

template<class T> Void TransferObject(ITransfer* transfer, List<T>& list)
{
    auto size = list.size();
    transfer->Transfer((UInt8*) &size, sizeof(UInt));
    if (transfer->IsReading())
        list.resize(size);
    if (size != 0)
        transfer->Transfer((UInt8*) list.data(), sizeof(T) * size);
}

Void TransferObject(ITransfer* transfer, Directory& directory)
{
    auto size = directory.GetSize();
    transfer->Transfer((UInt8*) &size, sizeof(UInt));
    if (size != 0)
        transfer->Transfer((UInt8*) &directory, sizeof(UInt8) * size);
}

Void TransferObject(ITransfer* transfer, Guid& Guid)
{
    transfer->Transfer((UInt8*) &Guid, sizeof(Guid));
}

Void Crate::Transfer(ITransfer* transfer)
{
    TransferObject(transfer, guid);
    TransferObject(transfer, externs);
    TransferObject(transfer, locals);
    TransferObject(transfer, globals);
}

enum class ResourceType : UInt32
{
    Local,
    Extern,
};

static List<ResourceExtern>::const_iterator FindExtern(const List<ResourceExtern>& resources, const Transferable* transferable)
{
    auto it = resources.cbegin();
    for (; it != resources.cend(); ++it)
    {
        auto& resource = *it;
        if (resource.id == *transferable->id)
            return it;
    }
    return it;
}

static List<ResourceLocal>::const_iterator FindLocal(const List<ResourceLocal>& resources, const Transferable* transferable)
{
    auto it = resources.cbegin();
    for (; it != resources.cend(); ++it)
    {
        auto& resource = *it;
        if (resource.cachedTransferable == transferable)
            return it;
    }
    return it;
}

static List<ResourceGlobal>::const_iterator FindGlobal(const List<ResourceGlobal>& resources, const Transferable* transferable)
{
    auto it = resources.cbegin();
    for (; it != resources.cend(); ++it)
    {
        auto& resource = *it;
        if (resource.id == *transferable->id)
            return it;
    }
    return it;
}

const Crate* CrateModule::AllocateCrate()
{
    return new Crate();
}

Void CrateModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    ExecuteBefore<TransfererModule>(moduleManager, transferers);
}

const Transferable* CrateModule::FindResource(const TransferableId* id)
{
    // TODO: Lock

    for (auto crate : crates)
    {
        for (auto& resource : crate->globals)
        {
            if (resource.id == *id)
            {
                auto& resourceLocal = crate->locals[resource.localIndex];
                if (resourceLocal.cachedTransferable == nullptr)
                    resourceLocal.cachedTransferable = LoadLocalResource(crate, resource.localIndex);
                return resourceLocal.cachedTransferable;
            }
        }
    }
    return nullptr;
}

SERIALIZE_METHOD_ARG2(CrateModule, Link, const Crate*, const Crate*);
SERIALIZE_METHOD_ARG3(CrateModule, AddTransferable, const Crate*, const Directory&, const Transferable*);
SERIALIZE_METHOD_ARG2(CrateModule, Save, const Directory&, const Crate*);
SERIALIZE_METHOD_ARG1(CrateModule, Load,  const Directory&);

Bool CrateModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG2_START(Link, Crate*, crate, const Crate*, externalCrate);
        crate->externalCrates.push_back(externalCrate);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(AddTransferable, Crate*, crate, const Directory, transferableDirectory, const Transferable*, transferable);
        AddTransferable(crate, transferableDirectory, transferable);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(Save, const Directory, directory, Crate*, crate);
        crate->directory = directory;
        Save(crate);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Load, const Directory, directory);
        auto crate = const_cast<Crate*>(AllocateCrate()); // TODO: make it re-usable
        crate->directory = directory;
        if (Load(crate))
        {
            crates.push_back(crate);
        }
        else
        {
            // TODO: Destroy
        }
        DESERIALIZE_METHOD_END;
    }
    return false;
}

const Crate* CrateModule::FindCrate(const Guid& guid) const
{
    for (auto crate : crates)
        if (crate->guid == guid)
            return crate;
    return nullptr;
}

const TransfererModule* CrateModule::FindTransferer(const TransfererId& id) const
{
    for (auto transferer : transferers)
        if (transferer->GetTransfererId() == id)
            return transferer;
    return nullptr;
}

Void CrateModule::Save(Crate* crate)
{
    // Generate unique guid that will be used for resolving external crates
    crate->guid = Guid::GetUnique();

    FileStream stream;
    CHECK(stream.Open(crate->directory.ToCString(), FileMode::Create, FileAccess::Write));

    // We have to calculate the header size and offset the stream, 
    // because we need to firstly resolve each resource offset and only then write the header
    TransferFindSize findSize;
    findSize.TransferPointer((Transferable*&) crate);
    stream.SetPosition(findSize.size);

    class TransferCrateWritter : public ITransfer
    {
    public:
        TransferCrateWritter(IO::Stream* stream, Crate* crate) : stream(stream), crate(crate) {}
        virtual Void Transfer(UInt8* data, UInt size) override { stream->Write(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override
        {
            auto external = FindExtern(crate->externs, transferable);
            if (external != crate->externs.cend())
            {
                auto type = ResourceType::Extern;
                Transfer((UInt8*) &type, sizeof(type));

                auto index = external - crate->externs.cbegin();
                Transfer((UInt8*) &index, sizeof(index));

                auto& id = transferable->GetTransfererId();
                Transfer((UInt8*) &id, sizeof(id));
            }

            auto local = FindLocal(crate->locals, transferable);
            if (local != crate->locals.cend())
            {
                auto type = ResourceType::Local;
                Transfer((UInt8*) &type, sizeof(type));

                auto index = local - crate->locals.cbegin();
                Transfer((UInt8*) &index, sizeof(index));

                auto& id = transferable->GetTransfererId();
                Transfer((UInt8*) &id, sizeof(id));
            }
        }
        virtual Bool IsReading() const override { return false; }

    public:
        IO::Stream* stream;
        Crate* crate;
    };

    // Data
    TransferCrateWritter dataWritter(&stream, crate);
    auto& resources = crate->locals;
    for (auto it = resources.rbegin(); it != resources.rend(); ++it) // Lets iterate from back, as this how it most like will be read
    {
        auto& resource = *it;
        dataWritter.TransferPointer((Transferable*&) resource.cachedTransferable);
        resource.offset = stream.GetPosition();
    }

    // Header
    stream.SetPosition(0);
    TransferBinaryWritter headerWritter(&stream);
    headerWritter.TransferPointer((Transferable*&) crate);

    stream.Close();
}

Bool CrateModule::Load(Crate* crate)
{
    FileStream stream;
    CHECK(stream.Open(crate->directory.ToCString(), FileMode::Open, FileAccess::Read));

    TransferBinaryReader reader(&stream);
    reader.TransferPointer((Transferable*&) crate);

    stream.Close();

    // Check if all external resources are reachable
    for (auto& resource : crate->externs)
    {
        resource.cachedCrate = FindCrate(resource.guid);
        if (resource.cachedCrate == nullptr)
            return false;
    }

    // Clear cached transferables as they will be filled during the FindResource
    for (auto& resource : crate->locals)
        resource.cachedTransferable = nullptr;

    for (auto& resource : crate->globals); // TODO: Add globals to map

    // We do not load the actual content now as we want loading only on demand
    // TODO: Still we should probably expose functionality to allow loading all content if needed

    return true;
}

const Transferable* CrateModule::LoadLocalResource(Crate* crate, UInt localIndex)
{
    FileStream stream;
    CHECK(stream.Open(crate->directory.ToCString(), FileMode::Open, FileAccess::Read));

    class TransferCrateDataReader : public ITransfer
    {
    public:
        TransferCrateDataReader(IO::Stream& stream, CrateModule* crateModule, Crate* crate) 
            : stream(stream), 
            crateModule(crateModule), 
            crate(crate) 
        {}
        virtual Void Transfer(UInt8* data, UInt size) override { stream.Write(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override
        {
            ResourceType type;
            Transfer((UInt8*) &type, sizeof(type));

            UInt index;
            Transfer((UInt8*) &index, sizeof(index));

            switch (type)
            {
            case ResourceType::Local:
                transferable = LoadLocal(crate, index);
                break;
            case ResourceType::Extern:
                transferable = LoadExtern(crate, index);
                break;
            default:
                ERROR("Unknown resource type with index %i", type);
                break;
            }

        }
        virtual Bool IsReading() const override { return false; }

        Transferable* LoadLocal(Crate* crate, UInt index)
        {
            auto& resource = crate->locals[index];
            stream.SetPosition(resource.offset);

            auto transferer = crateModule->FindTransferer(resource.transfererId);
            ASSERT(transferer != nullptr);

            auto transferable = const_cast<Transferable*>(transferer->AllocateTransferable());

            TransferPointer(transferable);

            return transferable;
        }

        Transferable* LoadExtern(Crate* crate, UInt index)
        {
            auto& resource = crate->externs[index];
            ASSERT(resource.cachedCrate != nullptr);
            return LoadLocal(const_cast<Crate*>(resource.cachedCrate), resource.index);
        }

    public:
        IO::Stream& stream;
        CrateModule* crateModule;
        Crate* crate;
    };

    TransferCrateDataReader dataReader(stream, this, crate);
    auto transferable = dataReader.LoadLocal(crate, localIndex);

    stream.Close();

    return transferable;
}

Void CrateModule::AddTransferable(Crate* crate, const Directory& directory, const Transferable* transferable)
{
    class TransferCrateHeader : public ITransfer
    {
    public:
        TransferCrateHeader(Crate* crate, const List<const Crate*>& externs) : crate(crate), externs(externs) {}
        virtual Bool IsReading() const override { return false; }
        virtual Void Transfer(UInt8* data, UInt size) override { }
        virtual Void TransferPointer(Transferable*& transferable) override
        {
            if (AddExternResource(transferable))
            {
                transferable->Transfer(this);
                return;
            }
            if (AddLocalResource(transferable))
            {
                transferable->Transfer(this);
                return;
            }
        }

        Bool AddExternResource(Transferable* transferable)
        {
            auto id = transferable->id;

            // If id is no valid it means it can not be external resource
            if (id == nullptr)
                return false;

            // Check if it is not already added
            for (auto& resource : crate->externs)
                if (resource.id == *id)
                    return false;

            // Try to find if resource is contained by external crates
            for (auto external : externs)
            {
                auto globalTransferable = FindGlobal(external->globals, transferable);
                if (globalTransferable != external->globals.end())
                {
                    ResourceExtern resource;
                    resource.id = *id;
                    resource.cachedCrate = external;
                    resource.index = globalTransferable - external->globals.begin();
                    crate->externs.push_back(resource);
                    return true;
                }
            }

            return false;
        }

        Bool AddLocalResource(Transferable* transferable)
        {
            // Check if it is not already added
            for (auto& resource : crate->locals)
                if (resource.cachedTransferable == transferable)
                    return false;

            ResourceLocal resource;
            resource.cachedTransferable = transferable;
            resource.transfererId = transferable->GetTransfererId();
            resource.offset = -1; // We will fill it in save as we will know the actual size of data
            crate->locals.push_back(resource);

            return true;
        }

    public:
        Crate* crate;
        const List<const Crate*>& externs;
    };

    TransferCrateHeader header(crate, crate->externalCrates);
    header.TransferPointer(const_cast<Transferable*&>(transferable)); // We can remove here safely as there will be no write

    // Add main transferable as global
    if (FindGlobal(crate->globals, transferable) == crate->globals.end())
    {
        auto& transferableInLocal = FindLocal(crate->locals, transferable);
        ASSERT(transferableInLocal != crate->locals.cend());

        ResourceGlobal resource;
        resource.localIndex = transferableInLocal - crate->locals.cbegin();
        resource.id.directory = directory;
        crate->globals.push_back(resource);
    }
}