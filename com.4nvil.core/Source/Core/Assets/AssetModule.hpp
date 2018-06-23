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
/*
#pragma once

#include <Core/Tools/Common.hpp>
#include <Core/Tools/Guid.hpp>
#include <Core/Tools/Collections/FixedBlockHeap.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Assets/Importers/IImporterSupportModule.hpp>

namespace Core
{
    struct ItemId
    {
        Directory directory;
    };

    enum class ItemType
    {
        Internal,
        External,
    };

    struct ItemExternal
    {
        Crate* crate;
    };

    struct ItemInternal
    {
        UInt8* offset;
        UInt size;
    };

    struct Item
    {
        ItemId id;
        ItemType type;
        union
        {
            ItemExternal external;
            ItemInternal internal;
        };
    };

    struct Crate
    {
        Directory directory;
        List<Item> items;
    };
    
    class CrateModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        const Crate* AllocateCrate();

    public:
        Void RecCreateCrate(const ExecutionContext& context, const Crate* crate, const Directory& directory, List<const Transferable*>& assets, List<Crate*>& includes);
        Void RecSaveCrate(const ExecutionContext& context, const Crate* crate);
        Void RecLoadCrate(const ExecutionContext& context, const Directory& directory);

        const Transferable* RecGetAsset(const ExecutionContext& context, const Directory& directory);

    private:
        List<Crate*> crates;
    };

    class AssetModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        Bool IsSupported(const Directory& directory) const;

    public:
        Void RecImport(const ExecutionContext& context, const Directory& directory, const Guid& guid);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        IImporterSupportModule* TryGetImporter(const Directory& directory) const;
        Void Import(const ExecutionContext& context, const Directory& directory, const Guid& guid);

    private:
        struct DirectoryToAsset
        {
            Directory directory;
            Guid guid;
            const Transferable* asset;
        };
        List<DirectoryToAsset> directoryToAsset;
        List<IImporterSupportModule*> importerModules;
    };
}*/