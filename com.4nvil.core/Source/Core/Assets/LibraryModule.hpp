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
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Foundation/TransferModule.hpp>

namespace Core
{
    class AssetModule;
}

namespace Core
{
    struct Tracked
    {
        Tracked() {}
        Tracked(const Directory& directory, const Guid& guid)
            : directory(directory)
            , guid(guid)
        { }
        Directory directory;
        Guid guid;
    };

    Void Transferer(ITransfer* transfer, const List<Tracked>& value);

    struct Library : public Transferable
    {
        virtual Void Transfer(ITransfer* transfer) override
        {
            Transferer(transfer, trackeds);
        }

        List<Tracked> trackeds;
    };

    class LibraryModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        const Library* AllocateLibrary();

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

    public:
        Void RecCreateLibrary(const ExecutionContext& context, const Library* target);
        Void RecTrack(const ExecutionContext& context, const Library* target, const Directory& directory);
        Void RecLoadLibrary(const ExecutionContext& context, const Library* target, const Directory& directory);
        Void RecSaveLibrary(const ExecutionContext& context, const Library* target, const Directory& directory);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void Track(const ExecutionContext& context, Library* target, const Directory& directory);
        Guid GenerateGuid();
        Bool Contains(const Library* target, const Directory& directory);

    private:
        AssetModule* assetModule;
        TransferModule* transferModule;
    };
}