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

#include <Core/Assets/ImporterModule.hpp>

using namespace Core;

Void ImporterModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    ExecuteBefore<IImporterSupportModule>(moduleManager, importerSupportModules);
}

Bool ImporterModule::IsSupported(const Directory& directory) const
{
    return TryGetImporter(directory) != nullptr;
}

SERIALIZE_METHOD_ARG1(ImporterModule, Import, const Directory&);

Bool ImporterModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(Import, const Directory, directory);
        Import(context, directory);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

IImporterSupportModule* ImporterModule::TryGetImporter(const Directory& directory) const
{
    DirectoryExtension extension;
    ASSERT(directory.GetExtension(extension));

    // Find first importer that supports given extension
    for (auto importerSupport : importerSupportModules)
        if (importerSupport->IsSupported(extension))
            return importerSupport;

    return nullptr;
}

Void ImporterModule::Import(const ExecutionContext& context, const Directory& directory)
{
    auto importer = TryGetImporter(directory);
    if (importer == nullptr)
        return;

    importer->RecImport(context, directory);
}