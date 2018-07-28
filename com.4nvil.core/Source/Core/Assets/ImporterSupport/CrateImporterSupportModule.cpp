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

#include <Core/Assets/CrateModule.hpp>
#include <Core/Assets/ImporterSupport/CrateImporterSupportModule.hpp>

using namespace Core;
using namespace Core::IO;

Void CrateImporterSupportModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    crateModule = ExecuteBefore<CrateModule>(moduleManager);
}

Bool CrateImporterSupportModule::IsSupported(const DirectoryExtension& extension)
{
    return extension == ".crate";
}

SERIALIZE_METHOD_ARG1(CrateImporterSupportModule, Import, const Directory&);
DECLARE_COMMAND_CODE(OnImport);

Bool CrateImporterSupportModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(Import, const Directory, directory);
        crateModule->RecLoad(context, directory, AsyncCallback<const Crate*>(this, TO_COMMAND_CODE(OnImport)));
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(OnImport, const Crate*, callback);
        TRACE("Very first callback 2 ptr=%d", callback);
        DESERIALIZE_METHOD_END;
    }
    return false;
}
