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
#include <Core/Tools/DateTime.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/CallbackModule.hpp>
#include <Core/Foundation/TransfererModule.hpp>

namespace Core
{
    struct Crate;
    class CrateModule;
}

namespace Core
{
    struct Library
    {
        Directory directory;
        DateTime createTime;
        Void* handle;
        List<Module*> modules;
        List<Library*> dependencies;
        List<TransfererId> transfererIds;
    };

    class LibraryModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API const Library* AllocateLibrary();

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        // Load library from the hard disk
        CORE_API Void RecLoad(const ExecutionContext& context, const Directory& directory, AsyncCallback<const Library*>& callback = AsyncCallback<const Library*>());

        // Unloads library and all libraries that depend on it
        CORE_API Void RecDestroy(const ExecutionContext& context, const Library* library);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Bool Load(const ExecutionContext& context, Library* library);
        Bool CanLoad(const ExecutionContext& context, const Directory& directory);
        const Library* FindLibrary(const Directory& directory) const;
        Void Destroy(const ExecutionContext& context, Library* library);

        Bool IsConnected(const Crate* crate, Library* library);

    private:
        List<Library*> libraries;
        ModuleManager* moduleManager;
        CrateModule* crateModule;
    };
}