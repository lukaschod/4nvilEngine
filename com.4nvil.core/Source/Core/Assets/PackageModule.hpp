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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/PipeModule.hpp>

namespace Core
{
    class ImporterModule;
}

namespace Core
{
    struct Package
    {
        Directory directory;
    };

    class PackageModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        CORE_API const Package* AllocatePackage();

    public:
        CORE_API Void RecCreatePackage(const ExecutionContext& context, const Package* target, const Directory& directory);
        CORE_API Void RecSyncPackage(const ExecutionContext& context, const Package* target);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void SyncPackage(const ExecutionContext& context, Package* package);
        Void SyncPackageDirectory(const ExecutionContext& context, Package* package, const Directory& directory);

    private:
        ImporterModule* importerModule;
        List<Package*> packages;
    };
}