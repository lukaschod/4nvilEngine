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
#include <Core/Tools/String.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/CallbackModule.hpp>
#include <Core/Foundation/TransfererModule.hpp>

namespace Core
{
    struct Transferable;
    class ImporterModule;
}

namespace Core
{
    class IImporterSupportModule : public CallbackModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        // Checks if extension is supported by this importer
        // Extension must be with dot (etc .png) and lower case
        CORE_API virtual Bool IsSupported(const DirectoryExtension& extension) pure;

    public:
        CORE_API virtual Void RecImport(const ExecutionContext& context, const Directory& directory) pure;

    protected:
        ImporterModule* importerModule;
    };
}