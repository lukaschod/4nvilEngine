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

#include <Core/Foundation/ComputeModule.hpp>
#include <Core/Assets/ImporterSupport/DllImporterSupportModule.hpp>

using namespace Core;

class PrintShitModule : public ComputeModule
{
public:
    BASE_IS(ComputeModule);

    virtual Void Execute(const ExecutionContext& context) override
    {
        TRACE("TestModule::Execute");
    }
};

Void DllImporterSupportModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    this->moduleManager = moduleManager;
}

Bool DllImporterSupportModule::IsSupported(const DirectoryExtension& extension)
{
    return extension == ".dll";
}

SERIALIZE_METHOD_ARG1(DllImporterSupportModule, Import, const Directory&);

Bool DllImporterSupportModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(Import, const Directory, directory);
        //moduleManager->RecAddModule(context, new PrintShitModule());
        DESERIALIZE_METHOD_END;
    }
    return false;
}
