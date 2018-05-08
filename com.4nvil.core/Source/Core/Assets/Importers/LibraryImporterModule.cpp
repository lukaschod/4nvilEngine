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
#include <Core/Assets/LibraryModule.hpp>
#include <Core/Assets/TransferModule.hpp>
#include <Core/Assets/Importers/LibraryImporterModule.hpp>

using namespace Core;
using namespace Core::IO;

Void LibraryImporterModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    libraryModule = ExecuteBefore<LibraryModule>(moduleManager);
    transferModule = ExecuteBefore<TransferModule>(moduleManager);
}

const char* LibraryImporterModule::GetSupportedExtension() { return ".l"; }

Void LibraryImporterModule::RecImport(const ExecutionContext& context, const Directory& directory)
{
    auto library = libraryModule->AllocateLibrary();

    auto stream = FileStream();
    stream.Open(directory.data, FileMode::Open, FileAccess::Read);
    transferModule->Transfer(library);
    stream.Close();

    libraryModule->RecCreateLibrary(context, library);
}

Bool LibraryImporterModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    return Bool();
}
