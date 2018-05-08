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
#include <Core/Assets/AssetModule.hpp>

using namespace Core;
using namespace Core::IO;

Void LibraryModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    assetModule = ExecuteBefore<AssetModule>(moduleManager);
}

const Library* LibraryModule::AllocateLibrary()
{
    return new Library();
}

SERIALIZE_METHOD_ARG1(LibraryModule, CreateLibrary, const Library*);
SERIALIZE_METHOD_ARG2(LibraryModule, Track, const Library*, const Directory&);
SERIALIZE_METHOD_ARG2(LibraryModule, LoadLibrary, const Library*, const Directory&);

Bool LibraryModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateLibrary, Library*, target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(Track, Library*, target, const Directory, directory);
        Track(context, target, directory);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(LoadLibrary, Library*, target, const Directory, directory);
        FileStream stream;
        stream.Open(directory.ToCString(), FileMode::Open, FileAccess::Read);
        transferModule->Transfter(target, stream);
        stream.Close();
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void LibraryModule::Track(const ExecutionContext& context, Library* target, const Directory& directory)
{
    if (!assetModule->IsSupported(directory))
        return;

    if (Contains(target, directory))
        return;

    auto guid = GenerateGuid();
    auto& trackeds = target->trackeds;
    trackeds.push_back(Tracked(directory, guid));

    assetModule->RecImport(context, directory, guid);
}

Guid LibraryModule::GenerateGuid()
{
    return Guid();
}

Bool LibraryModule::Contains(const Library* target, const Directory& directory)
{
    for (auto& tracked : target->trackeds)
    {
        if (tracked.directory == directory)
            return true;
    }
    return false;
}
