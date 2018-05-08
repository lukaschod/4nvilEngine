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

#include <Core/Assets/PackageModule.hpp>
#include <Core/Assets/LibraryModule.hpp>

using namespace Core;

Void PackageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    libraryModule = ExecuteBefore<LibraryModule>(moduleManager);
}

const Package* PackageModule::AllocatePackage()
{
    auto package = new Package();
    package->library = libraryModule->AllocateLibrary();
    return package;
}

SERIALIZE_METHOD_ARG1(PackageModule, CreatePackage, const Package*);
SERIALIZE_METHOD_ARG1(PackageModule, SyncPackage, const Package*);

Bool PackageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
    DESERIALIZE_METHOD_ARG1_START(CreatePackage, Package*, target);
    libraryModule->RecCreateLibrary(context, target->library);
    packages.push_back(target);
    DESERIALIZE_METHOD_END;

    DESERIALIZE_METHOD_ARG1_START(SyncPackage, Package*, package);
    SyncPackage(context, package);
    DESERIALIZE_METHOD_END;
    }
    return false;
}

Void PackageModule::SyncPackage(const ExecutionContext& context, Package* package)
{
    SyncPackageLibrary(context, package, package->directory);
    SyncPackageDirectory(context, package, package->directory);
    libraryModule->RecSaveLibrary(context, package->directory);
}

Void PackageModule::SyncPackageDirectory(const ExecutionContext& context, Package* package, const Directory& directory)
{
    List<Directory> directories; // reuse it
    Directory::GetDirectories(directory, directories);

    for (auto directory : directories)
    {
        if (directory.IsFile())
            libraryModule->RecTrack(context, package->library, directory);
        else
            SyncPackageDirectory(context, package, directory);
    }
}

Void PackageModule::SyncPackageLibrary(const ExecutionContext& context, Package* package, const Directory& directory)
{
    List<Directory> directories; // reuse it
    Directory::GetDirectories(directory, directories);

    for (auto directory : directories)
    {
        if (directory.IsFile())
        {
            DirectoryExtension extension;
            ASSERT(directory.GetExtension(extension));
            if (extension == DirectoryExtension(L".library"))
                libraryModule->RecLoadLibrary(context, package->library, directory);
        }
        else
            SyncPackageDirectory(context, package, directory);
    }
}
