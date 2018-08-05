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

#include <Core/Tools/Character.hpp>
#include <Core/Tools/IO/FileStream.hpp>
#include <Core/Assets/CrateModule.hpp>
#include <Core/Assets/TransferCrateBinary.h>
#include <Core/Assets/TransferCrateJSON.h>
#include <Core/Foundation/TransfererModule.hpp>
#include <Core/Foundation/TransfererUtility.hpp>

using namespace Core;
using namespace Core::IO;

namespace Core
{
    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, ResourceExtern& value)
    {
        TRANSFER(value.id);
        TRANSFER(value.guid);
        TRANSFER(value.globalIndex);
    }

    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, ResourceLocal& value)
    {
        TRANSFER(value.transfererId);
        transfer->Transfer("offset", &value.offset, sizeof(value.offset));
        TRANSFER(value.dependencies);
    }

    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, ResourceGlobal& value)
    {
        TRANSFER(value.id);
        TRANSFER(value.localIndex);
    }

    template<> inline Void TransferValue(ITransfer* transfer, const Char* name, Crate*& value)
    {
        TRANSFER(value->guid);
        TRANSFER(value->externs);
        TRANSFER(value->locals);
        TRANSFER(value->globals);
    }
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
    cachedLoadCrate = nullptr;
}

SERIALIZE_METHOD_ARG2(CrateModule, Link, const Crate*, const Crate*);
SERIALIZE_METHOD_ARG3(CrateModule, AddTransferable, const Crate*, const Directory&, const Transferable*);
SERIALIZE_METHOD_ARG2(CrateModule, Save, const Directory&, const Crate*);
SERIALIZE_METHOD_ARG2(CrateModule, Load, const Directory&, AsyncCallback<const Crate*>&);
SERIALIZE_METHOD_ARG1(CrateModule, LoadResource, const TransferableId&);
SERIALIZE_METHOD_ARG1(CrateModule, Destroy, const Crate*);

Bool CrateModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG2_START(Link, Crate*, crate, Crate*, externalCrate);
        crate->dependencies.push_back(externalCrate);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(AddTransferable, Crate*, crate, const Directory, transferableDirectory, const Transferable*, transferable);
        AddTransferable(crate, transferableDirectory, transferable);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(Save, const Directory, directory, Crate*, crate);
        crate->directory = directory;
        crate->guid = Guid::Generate();
        crate->createTime = DateTime::Now();
        strcpy(crate->transferName, "json.1");
        Save(crate);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(Load, const Directory, directory, AsyncCallback<const Crate*>, callback);
        if (CanLoad(context, directory))
        {
            // Cache load crate as it might fail so there is no point to always allocate new one
            if (cachedLoadCrate == nullptr)
                cachedLoadCrate = const_cast<Crate*>(AllocateCrate());;
            cachedLoadCrate->directory = directory;
            CHECK(directory.GetWriteTime(cachedLoadCrate->createTime));

            // Try load, it might fail because of dependencies
            if (Load(cachedLoadCrate))
            {
                crates.push_back(cachedLoadCrate);
                callback.IssueCallback(context, (const Crate*) cachedLoadCrate);
                cachedLoadCrate = nullptr;
            }
            else
                callback.IssueCallback(context, (const Crate*) nullptr);
        }
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(LoadResource, const TransferableId, id);
        LoadResource(context, id);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, const Crate*, crate);
        
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void CrateModule::LoadResource(const ExecutionContext& context, const TransferableId& id)
{
    // TODO: Directory
    for (auto crate : crates)
    {
        for (UInt i = 0; i < crate->globals.size(); i++)
        {
            auto& resource = crate->globals[i];
            if (resource.id == id)
            {
                auto& resourceLocal = crate->locals[resource.localIndex];

                // Early out if it is already loaded
                if (resourceLocal.cachedTransferable)
                    return;

                LoadResource(context, crate, i);
            }
        }
    }
}

TransferCrateBinaryWritter* CrateModule::TryGetWritter(const Char* name)
{
    if (Character::Equals(name, "binary.1"))
    {
        static TransferCrateBinaryWritter transfer;
        return &transfer;
    }

    if (Character::Equals(name, "json.1"))
    {
        static TransferCrateJSONWritter transfer;
        return &transfer;
    }

    return nullptr;
}

TransferCrateBinaryReader* CrateModule::TryGetReader(const Char* name)
{
    if (Character::Equals(name, "binary.1"))
    {
        static TransferCrateBinaryReader transfer;
        return &transfer;
    }

    if (Character::Equals(name, "json.1"))
    {
        static TransferCrateJSONReader transfer;
        return &transfer;
    }

    return nullptr;
}

Void CrateModule::Destroy(const ExecutionContext& context, Crate* crate)
{
    // Destroy all crates that depends on it
    for (auto other : crates)
    {
        if (IsConnected(other, crate))
            Destroy(context, other);
    }

    // TODO: Destroy all depending transferables
    for (auto& resource : crate->globals)
    {
        auto transferable = const_cast<Transferable*>(crate->locals[resource.localIndex].cachedTransferable);
        transferable->id = nullptr;
    }

    crates.remove(crate);
    delete crate;
}

Bool CrateModule::IsConnected(Crate* source, Crate* destination)
{
    for (auto& resource : source->externs)
        if (resource.cachedCrate == destination)
            return true;
    return false;
}

const Crate* CrateModule::FindCrate(const Guid& guid) const
{
    for (auto crate : crates)
        if (crate->guid == guid)
            return crate;
    return nullptr;
}

const Crate* CrateModule::FindCrate(const Directory& directory) const
{
    for (auto crate : crates)
        if (crate->directory == directory)
            return crate;
    return nullptr;
}

TransfererModule* CrateModule::FindTransferer(const TransfererId& id) const
{
    for (auto transferer : transferers)
        if (transferer->GetTransfererId() == id)
            return transferer;
    return nullptr;
}

Void CrateModule::Save(Crate* crate)
{
    FileStream stream;
    CHECK(stream.Open(crate->directory.ToCString(), FileMode::Create, FileAccess::Write));

    auto transfer = TryGetWritter(crate->transferName);
    CHECK(transfer != nullptr);
    transfer->Reset(&stream);

    stream.WriteFmt("#crate.%20s\n", transfer->GetName());
    auto headerOffset = stream.GetPosition();

    // Write fake one, just to offset to correct point
    TransferValue(transfer, "header", crate);

    auto localResourcesOffset = stream.GetPosition();

    auto& resources = crate->locals;
    for (UInt i = 0; i < resources.size(); i++)
        transfer->WriteLocalResource(crate, i);

    // As now all offset values are correct, we write correct header now
    stream.SetPosition(headerOffset);
    TransferValue(transfer, "header", crate);

    ASSERT_MSG(localResourcesOffset == stream.GetPosition(), "Incorrect header size");

    stream.Close();

    // Assign to all global resources their ids
    for (auto& global : crate->globals)
    {
        auto transferable = const_cast<Transferable*>(crate->locals[global.localIndex].cachedTransferable);
        transferable->id = &global.id;
    }
}

Bool CrateModule::CanLoad(const ExecutionContext& context, const Directory& directory)
{
    auto crate = const_cast<Crate*>(FindCrate(directory));
    if (crate != nullptr)
    {
        DateTime createTime;
        directory.GetWriteTime(createTime);
        if (crate->createTime < createTime)
        {
            Destroy(context, crate);
            return true;
        }

        return false;
    }
    return true;
}

Bool CrateModule::Load(Crate* crate)
{
    FileStream stream;
    CHECK(stream.Open(crate->directory.ToCString(), FileMode::Open, FileAccess::Read));

    stream.ReadFmt("#crate.%20s\n", crate->transferName);

    auto transfer = TryGetReader(crate->transferName);
    if (transfer == nullptr)
    {
        ERROR("Can not find transfer");
        return false;
    }
    transfer->Reset(&stream, nullptr, this);

    TransferValue(transfer, "header", crate);

    stream.Close();

    // Check if all external resources are reachable
    for (auto& resource : crate->externs)
    {
        resource.cachedCrate = FindCrate(resource.guid);
        if (resource.cachedCrate == nullptr)
            return false;
    }

    for (auto& resource : crate->locals)
    {
        // Clear cached transferables as they will be filled during the FindResource
        resource.cachedTransferable = nullptr;

        // Check if all transferers available
        auto transferer = FindTransferer(resource.transfererId);
        if (transferer == nullptr)
            return false;
    }

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

    // TODO: multi-thread it will be easy, as all of them can be executed concurrently
    // Read each transferable one by one
    CrateDependancySolver dependancySolver;

    Crate* cachedCrate = nullptr;
    FileStream cachedStream;
    TransferCrateBinaryReader* cachedTransfer = nullptr;

    for (auto& resource : resourceSolver.resources)
    {
        // Check if we need different stream
        if (cachedCrate != resource.crate)
        {
            cachedCrate = const_cast<Crate*>(resource.crate);
            if (cachedStream.IsOpened())
                cachedStream.Close();
            CHECK(cachedStream.Open(cachedCrate->directory.ToCString(), FileMode::Open, FileAccess::Read));

            cachedTransfer = TryGetReader(cachedCrate->transferName);
            CHECK(cachedTransfer != nullptr);
            cachedTransfer->Reset(&cachedStream, &dependancySolver, this);
        }

        cachedTransfer->ReadLocalResource(context, cachedCrate, resource.localIndex);
    }

    // Close last cached stream
    if (cachedStream.IsOpened())
        cachedStream.Close();

    // Update the pointers to actual transferables
    dependancySolver.Solve();
}

Void CrateModule::AddTransferable(Crate* crate, const Directory& directory, const Transferable* transferable)
{
    class TransferCrateHeader : public ITransfer
    {
    public:
        TransferCrateHeader(Crate* crate, const List<Crate*>& externs) : crate(crate), externs(externs), currentLocalResourceIndex(-1) {}
        virtual Void TransferPointer(const Char* name, Transferable*& transferable) override
        {
            if (transferable == nullptr)
                return;

            if (AddExternResource(transferable))
            {
                return;
            }
            if (AddLocalResource(transferable))
            {
                auto lastResourceIndex = currentLocalResourceIndex;
                currentLocalResourceIndex = crate->locals.size() - 1;
                transferable->Transfer(this);
                currentLocalResourceIndex = lastResourceIndex;
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
            auto& externResources = crate->externs;
            for (auto it = externResources.cbegin(); it != externResources.cend(); ++it)
                if (it->id == *id)
                {
                    AddDependancy(ResourceType::Extern, it - externResources.cbegin());
                    return false;
                }

            // Try to find if resource is contained by external crates
            for (auto external : externs)
            {
                auto globalTransferable = FindGlobal(external->globals, transferable);
                if (globalTransferable != external->globals.end())
                {
                    AddDependancy(ResourceType::Extern, externResources.size());

                    ResourceExtern resource;
                    resource.id = *id;
                    resource.guid = external->guid;
                    resource.globalIndex = globalTransferable - external->globals.begin();
                    resource.cachedCrate = external;
                    externResources.push_back(resource);

                    return true;
                }
            }

            return false;
        }

        Bool AddLocalResource(Transferable* transferable)
        {
            // Check if it is not already added
            auto& locals = crate->locals;
            for (auto it = locals.cbegin(); it != locals.cend(); ++it)
                if (it->cachedTransferable == transferable)
                {
                    AddDependancy(ResourceType::Local, it - locals.cbegin());
                    return false;
                }

            AddDependancy(ResourceType::Local, locals.size());

            ResourceLocal resource;
            resource.cachedTransferable = transferable;
            resource.transfererId = transferable->GetTransfererId();
            resource.offset = -1; // We will fill it in save as we will know the actual size of data
            locals.push_back(resource);

            return true;
        }

        Void AddDependancy(ResourceType type, UInt index)
        {
            if (currentLocalResourceIndex != -1)
                crate->locals[currentLocalResourceIndex].dependencies.push_back(ResourceDependancy(type, index));
        }

    public:
        Crate* crate;
        UInt currentLocalResourceIndex;
        const List<Crate*>& externs;
    };

    TransferCrateHeader header(crate, crate->dependencies);
    header.TransferPointer("", const_cast<Transferable*&>(transferable)); // We can remove here safely as there will be no write

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