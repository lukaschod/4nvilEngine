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

#include <Core/Tools/IO/FileStream.hpp>
#include <Core/Assets/CrateModule.hpp>
#include <Core/Assets/ImporterSupport/UnitImporterSupportModule.hpp>

using namespace Core;
using namespace Core::IO;

Void UnitImporterSupportModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    crateModule = ExecuteBefore<CrateModule>(moduleManager);
}

Bool UnitImporterSupportModule::IsSupported(const DirectoryExtension& extension)
{
    return extension == ".crate";
}

SERIALIZE_METHOD_ARG1(UnitImporterSupportModule, Import, const Directory&);
SERIALIZE_METHOD_ARG2(UnitImporterSupportModule, Export, const Directory&, const Transferable*);

Bool UnitImporterSupportModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(Import, const Directory, directory);
        crateModule->RecLoad(context, directory);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(Export, const Directory, directory, Transferable*, target);
        DESERIALIZE_METHOD_END;
    }
    return false;
}
