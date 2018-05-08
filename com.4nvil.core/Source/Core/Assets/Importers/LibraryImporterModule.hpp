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
#include <Core/Assets/Importers/IImporterModule.hpp>

namespace Core
{
    class LibraryModule;
    class TransferModule;
}

namespace Core
{
    class LibraryImporterModule : public IImporterModule
    {
    public:
        BASE_IS(IImporterModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual const char* GetSupportedExtension() override;

    public:
        virtual Void RecImport(const ExecutionContext& context, const Directory& directory) override;

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        LibraryModule* libraryModule;
        TransferModule* transferModule;
    };
}