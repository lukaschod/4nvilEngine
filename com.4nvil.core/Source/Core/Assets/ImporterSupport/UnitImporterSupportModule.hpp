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
#include <Core/Assets/IImporterSupportModule.hpp>

namespace Core
{
    struct Transferable;
    class CrateModule;
}

namespace Core
{
    class UnitImporterSupportModule : public IImporterSupportModule
    {
    public:
        BASE_IS(IImporterSupportModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual Bool IsSupported(const DirectoryExtension& extension) override;

    public:
        virtual Void RecImport(const ExecutionContext& context, const Directory& directory) override;
        virtual Void RecExport(const ExecutionContext& context, const Directory& directory, const Transferable* target);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        CrateModule* crateModule;
    };
}