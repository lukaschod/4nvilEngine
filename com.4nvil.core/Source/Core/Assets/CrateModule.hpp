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
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Foundation/TransferModule.hpp>

namespace Core
{
    struct Crate;
}

namespace Core
{
    struct ResourceExtern
    {
        TransferableId id;
        Guid guid;
        UInt index;

        const Crate* cachedCrate;
    };

    struct ResourceLocal
    {
        TransfererId transfererId;
        UInt offset;

        const Transferable* cachedTransferable;
    };

    struct ResourceGlobal
    {
        TransferableId id;
        UInt localIndex;
    };

    struct Crate : public Transferable
    {
        IMPLEMENT_TRANSFERABLE(Crate);

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

        const Transferable* FindResource(const TransferableId* id);
        const TransfererModule* FindTransferer(const TransfererId& id) const;
        const Crate* FindCrate(const Guid& guid) const;

    public:
        // Adds include to the crate that will be used for resolving already serialized transferables
        Void RecLink(const ExecutionContext& context, const Crate* crate, const Crate* externalCrate);

        // Adds transferable to crate
        Void RecAddTransferable(const ExecutionContext& context, const Crate* crate, const Directory& transferableDirectory, const Transferable* transferable);

        // Serializes crate into directory
        Void RecSave(const ExecutionContext& context, const Directory& directory, const Crate* crate);

        // Deserializes crate from directory
        Void RecLoad(const ExecutionContext& context, const Directory& directory);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void Save(Crate* crate);
        Bool Load(Crate* crate);
        Void AddTransferable(Crate* crate, const Directory& directory, const Transferable* transferable);
        const Transferable* LoadLocalResource(Crate* crate, UInt localIndex);

    private:
        List<Crate*> crates;
        List<TransfererModule*> transferers;
    };
}