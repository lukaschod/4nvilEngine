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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/Collections/Dictonary.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/CallbackModule.hpp>
#include <Core/Foundation/TransfererModule.hpp>

namespace Core
{
    struct Crate;
    class TransferCrateBinaryReader;
    class TransferCrateBinaryWritter;
}

namespace Core
{
    struct ResourceExtern
    {
        TransferableId id;
        Guid guid;
        UInt globalIndex;

        const Crate* cachedCrate;
    };

    enum class ResourceType : UInt32
    {
        Local,
        Extern,
    };

    struct ResourceDependancy
    {
        ResourceDependancy() {}
        ResourceDependancy(ResourceType type, UInt index) : type(type), index(index) {}
        ResourceType type;
        UInt index;
    };

    struct ResourceLocal
    {
        TransfererId transfererId;
        UInt offset;
        List<ResourceDependancy> dependencies;

        const Transferable* cachedTransferable;
    };

    struct ResourceGlobal
    {
        TransferableId id;
        UInt localIndex;
    };

    struct Crate
    {
        Guid guid;
        Directory directory;
        DateTime createTime;
        Char transferName[20];
        List<ResourceExtern> externs;
        List<ResourceLocal> locals;
        List<ResourceGlobal> globals;
        List<Crate*> dependencies;
    };

    class CrateModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API const Crate* AllocateCrate();

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        // Find transferer from id
        CORE_API TransfererModule* FindTransferer(const TransfererId& id) const;

    public:
        // Adds include to the crate that will be used for resolving already serialized transferables
        CORE_API Void RecLink(const ExecutionContext& context, const Crate* crate, const Crate* externalCrate);

        // Adds transferable to crate
        CORE_API Void RecAddTransferable(const ExecutionContext& context, const Crate* crate, const Directory& transferableDirectory, const Transferable* transferable);

        // Serializes crate into directory
        CORE_API Void RecSave(const ExecutionContext& context, const Directory& directory, const Crate* crate);

        // Deserializes crate from directory
        CORE_API Void RecLoad(const ExecutionContext& context, const Directory& directory, AsyncCallback<const Crate*>& callback = AsyncCallback<const Crate*>());

        // Find resource from the loaded crates, it might cause resource loading
        CORE_API Void RecLoadResource(const ExecutionContext& context, const TransferableId& id);

        CORE_API Void RecDestroy(const ExecutionContext& context, const Crate* crate);

        inline const List<const Crate*>& GetCrates() const { return (List<const Crate*>&)crates; }

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void Save(Crate* crate);
        Bool Load(Crate* crate);
        Bool CanLoad(const ExecutionContext& context, const Directory& directory);
        Void AddTransferable(Crate* crate, const Directory& directory, const Transferable* transferable);
        Void LoadResource(const ExecutionContext& context, Crate* crate, UInt globalIndex);
        Void LoadResource(const ExecutionContext& context, const TransferableId& id);
        TransferCrateBinaryWritter* TryGetWritter(const Char* name);
        TransferCrateBinaryReader* TryGetReader(const Char* name);
        Void Destroy(const ExecutionContext& context, Crate* crate);

        // Checks if source depends on destination
        Bool IsConnected(Crate* source, Crate* destination);

        const Crate* FindCrate(const Guid& guid) const;
        const Crate* FindCrate(const Directory& directory) const;

    private:
        List<Crate*> crates;
        List<TransfererModule*> transferers;
        Crate* cachedLoadCrate;
    };
}