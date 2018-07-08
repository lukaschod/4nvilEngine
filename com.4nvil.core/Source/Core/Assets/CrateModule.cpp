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
#include <Core/Assets/CrateSolvers.h>
#include <Core/Foundation/TransfererModule.hpp>
#include <Core/Foundation/TransfererUtility.hpp>

using namespace Core;
using namespace Core::IO;

namespace Core
{
    template<> inline Void TransferValue(ITransfer* transfer, ResourceExtern& value)
    {
        TRANSFER(value.id);
        TRANSFER(value.guid);
        TRANSFER(value.globalIndex);
    }

    template<> inline Void TransferValue(ITransfer* transfer, ResourceDependancy& value)
    {
        TRANSFER(value.type);
        TRANSFER(value.index);
    }

    template<> inline Void TransferValue(ITransfer* transfer, ResourceLocal& value)
    {
        TRANSFER(value.transfererId);
        TRANSFER(value.offset);
        TRANSFER(value.dependencies);
    }

    template<> inline Void TransferValue(ITransfer* transfer, ResourceGlobal& value)
    {
        TRANSFER(value.id);
        TRANSFER(value.localIndex);
    }
}

Void Crate::Transfer(ITransfer* transfer)
{
    TRANSFER(guid);
    TRANSFER(externs);
    TRANSFER(locals);
    TRANSFER(globals);
}

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

SERIALIZE_METHOD_ARG2(CrateModule, Link, const Crate*, const Crate*);
SERIALIZE_METHOD_ARG3(CrateModule, AddTransferable, const Crate*, const Directory&, const Transferable*);
SERIALIZE_METHOD_ARG2(CrateModule, Save, const Directory&, const Crate*);
SERIALIZE_METHOD_ARG1(CrateModule, Load, const Directory&);
SERIALIZE_METHOD_ARG1(CrateModule, LoadResource, const TransferableId*);

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
            delete crate; // TODO
        }
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(LoadResource, const TransferableId*, id);
        LoadResource(context, id);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void CrateModule::LoadResource(const ExecutionContext& context, const TransferableId* id)
{
    for (auto crate : crates)
    {
        for (UInt i = 0; i < crate->globals.size(); i++)
        {
            auto& resource = crate->globals[i];

            // Early out if it is already loaded
            if (resource.id == *id)
            {
                auto& resourceLocal = crate->locals[resource.localIndex];
                if (resourceLocal.cachedTransferable)
                    return;
            }

            LoadResource(context, crate, i);
        }
    }
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
        TransferCrateWritter(IO::Stream& stream, Crate* crate) : stream(stream), crate(crate) {}
        virtual Void Transfer(Void* data, UInt size) override { stream.Write(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override
        {
            auto external = FindExtern(crate->externs, transferable);
            if (external != crate->externs.cend())
            {
                ResourceDependancy dependancy;
                dependancy.type = ResourceType::Extern;
                dependancy.index = external - crate->externs.cbegin();
                currentResource->dependencies.push_back(dependancy);

                Transfer(&dependancy, sizeof(dependancy));
            }

            auto local = FindLocal(crate->locals, transferable);
            if (local != crate->locals.cend())
            {
                ResourceDependancy dependancy;
                dependancy.type = ResourceType::Local;
                dependancy.index = local - crate->locals.cbegin();
                currentResource->dependencies.push_back(dependancy);

                Transfer(&dependancy, sizeof(dependancy));
            }
        }
        virtual Bool IsWritting() const override { return true; }

        Void Transfer(ResourceLocal& resource)
        {
            currentResource = &resource;
            resource.offset = stream.GetPosition();

            auto transferable = const_cast<Transferable*>(resource.cachedTransferable);
            ASSERT(transferable != nullptr);
            transferable->Transfer(this);
        }

    public:
        IO::Stream& stream;
        Crate* crate;
        ResourceLocal* currentResource;
    };

    // Data
    TransferCrateWritter dataWritter(stream, crate);
    auto& resources = crate->locals;
    for (auto& resource : resources)
        dataWritter.Transfer(resource);

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

Void CrateModule::LoadResource(const ExecutionContext& context, Crate* crate, UInt globalIndex)
{
    // Find resources that will be used for transfering
    CrateResourceSolver resourceSolver;
    resourceSolver.Solve(crate, globalIndex);

    class TransferCrateDataReader : public ITransfer
    {
    public:
        TransferCrateDataReader(const ExecutionContext& context, CrateDependancySolver& dependancySolver, CrateModule* crateModule)
            : context(context)
            , crateModule(crateModule)
            , dependancySolver(dependancySolver)
        {}
        virtual Void Transfer(Void* data, UInt size) override { cachedStream.Read(data, size); }
        virtual Void TransferPointer(Transferable*& transferable) override
        {
            ResourceDependancy resource;
            Transfer(&resource, sizeof(resource));

            // Store dependancy, we will resolve it later on, once all the transferables are loaded
            dependancySolver.Add(cachedCrate, (const Transferable*&)transferable, resource);
        }
        virtual Bool IsReading() const override { return true; }

        Void Read(Crate* crate, UInt localIndex)
        {
            if (crate != cachedCrate)
            {
                Close(); // Close last stream
                cachedCrate = crate;
                CHECK(cachedStream.Open(cachedCrate->directory.ToCString(), FileMode::Open, FileAccess::Read));
            }

            auto& resource = cachedCrate->locals[localIndex];

            // Allocate transferable
            auto transferer = const_cast<TransfererModule*>(crateModule->FindTransferer(resource.transfererId));
            ASSERT(transferer != nullptr);
            auto transferable = const_cast<Transferable*>(transferer->AllocateTransferable());

            // Cache it
            resource.cachedTransferable = transferable;

            // Transfer its data
            cachedStream.SetPosition(resource.offset);
            transferable->Transfer(this);

            // Issue the create request
            transferer->RecCreateTransferable(context, transferable);
        }

        Void Close()
        {
            if (cachedStream.IsOpened())
                cachedStream.Close();
        }

    public:
        const ExecutionContext& context;
        CrateModule* crateModule;
        CrateDependancySolver& dependancySolver;
        FileStream cachedStream;
        Crate* cachedCrate;
    };

    // TODO: multi-thread it will be easy, as all of them can be executed concurrently
    // Read each transferable one by one
    CrateDependancySolver dependancySolver;
    TransferCrateDataReader dataReader(context, dependancySolver, this);
    for (auto& resource : resourceSolver.resources)
        dataReader.Read(const_cast<Crate*>(resource.crate), resource.localIndex);
    dataReader.Close();

    // Update the pointers to actual transferables
    dependancySolver.Solve();
}

Void CrateModule::AddTransferable(Crate* crate, const Directory& directory, const Transferable* transferable)
{
    class TransferCrateHeader : public ITransfer
    {
    public:
        TransferCrateHeader(Crate* crate, const List<const Crate*>& externs) : crate(crate), externs(externs) {}
        virtual Void Transfer(Void* data, UInt size) override { }
        virtual Void TransferPointer(Transferable*& transferable) override
        {
            if (transferable == nullptr)
                return;

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
                    resource.globalIndex = globalTransferable - external->globals.begin();
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