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

/*#include <Core/Assets/AssetModule.hpp>

using namespace Core;

Void AssetModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    ExecuteBefore<IImporterSupportModule>(moduleManager, importerModules);
}

Bool AssetModule::IsSupported(const Directory& directory) const
{
    return TryGetImporter(directory) != nullptr;
}

SERIALIZE_METHOD_ARG2(AssetModule, Import, const Directory&, const Guid&);

Bool AssetModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG2_START(Import, const Directory, directory, const Guid, guid);
        Import(context, directory, guid);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

IImporterSupportModule* AssetModule::TryGetImporter(const Directory& directory) const
{
    DirectoryExtension extension;
    ASSERT(directory.GetExtension(extension));

    for (auto importerModule : importerModules)
    {
        if (importerModule->IsSupported(extension))
            return importerModule;
    }
    return nullptr;
}

Void AssetModule::Import(const ExecutionContext& context, const Directory& directory, const Guid& guid)
{
    auto importer = TryGetImporter(directory);
    ASSERT(importer != nullptr);

    DirectoryToAsset d2a;
    d2a.asset = importer->Import(context, directory);
    d2a.directory = directory;
    d2a.guid = guid;
}
*/