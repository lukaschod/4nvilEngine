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
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Assets/IImporterSupportModule.hpp>

namespace Core
{
    class ImporterModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        Bool IsSupported(const Directory& directory) const;

    public:
        Void RecImport(const ExecutionContext& context, const Directory& directory);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        IImporterSupportModule* TryGetImporter(const Directory& directory) const;
        Void Import(const ExecutionContext& context, const Directory& directory);

    private:
        List<IImporterSupportModule*> importerSupportModules;
    };
}