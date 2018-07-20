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
#include <Core/Foundation/PipeModule.hpp>
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
        List<ResourceExtern> externs;
        List<ResourceLocal> locals;
        List<ResourceGlobal> globals;
        List<const Crate*> externalCrates;
    };

    class CrateModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        const Crate* AllocateCrate();

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        const TransfererModule* FindTransferer(const TransfererId& id) const;
        const Crate* FindCrate(const Guid& guid) const;
        const Crate* FindCrate(const Directory& directory) const;

    public:
        // Adds include to the crate that will be used for resolving already serialized transferables
        Void RecLink(const ExecutionContext& context, const Crate* crate, const Crate* externalCrate);

        // Adds transferable to crate
        Void RecAddTransferable(const ExecutionContext& context, const Crate* crate, const Directory& transferableDirectory, const Transferable* transferable);

        // Serializes crate into directory
        Void RecSave(const ExecutionContext& context, const Directory& directory, const Crate* crate);

        // Deserializes crate from directory
        Void RecLoad(const ExecutionContext& context, const Directory& directory);

        // Find resource from the loaded crates, it might cause resource loading
        Void RecLoadResource(const ExecutionContext& context, const TransferableId& id);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void Save(Crate* crate);
        Bool Load(Crate* crate);
        Void AddTransferable(Crate* crate, const Directory& directory, const Transferable* transferable);
        Void LoadResource(const ExecutionContext& context, Crate* crate, UInt globalIndex);
        Void LoadResource(const ExecutionContext& context, const TransferableId& id);
        TransferCrateBinaryWritter* TryGetWritter(const Char* name);
        TransferCrateBinaryReader* TryGetReader(const Char* name);

    private:
        List<Crate*> crates;
        List<TransfererModule*> transferers;
    };
}