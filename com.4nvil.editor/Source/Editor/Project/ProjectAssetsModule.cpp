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

/*#include <Core/Tools/IO/Directory.hpp>
#include <Editor/Project/ProjectAssetsModule.hpp>

using namespace Core;
using namespace Editor;

SERIALIZE_METHOD(ProjectAssetsModule, Synchronize);
SERIALIZE_METHOD_ARG1(ProjectAssetsModule, SetPath, String&);

Void ProjectAssetsModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    synchronizeIndex = 0;
}

Bool ProjectAssetsModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_START(Synchronize);
        synchronizeIndex++;
        Synchronize(&mainDirectory);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(SetPath, String, path);
        mainDirectory.path = path;
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void ProjectAssetsModule::Synchronize(ProjectDiretory* target)
{
    // Check if we need to add new directories from physical paths
    auto directories = Directory::GetDirectories(target->path);
    for (auto& directory : directories)
    {
        auto projectDirectory = TryFindDirectory(target, directory);
        if (projectDirectory == nullptr)
        {
            projectDirectory = new ProjectDiretory();
            projectDirectory->path = directory;
        }
        projectDirectory->synchronizeIndex = synchronizeIndex;
    }

    // Check if we need to add new files from physical paths
    auto files = Directory::GetFiles(target->path);
    for (auto& file : files)
    {
        auto projectFile = TryFindFile(target, file);
        if (projectFile == nullptr)
        {
            projectFile = new ProjectFile();
            projectFile->path = file;
        }
        projectFile->synchronizeIndex = synchronizeIndex;
    }

    // Remove directories that are out of sync
    for (int i = 0; i < target->directories.size(); i++)
    {
        if (target->directories[i]->synchronizeIndex != synchronizeIndex)
        {
            DestroyDirectoryRecursive(target->directories[i]);
            target->directories.erase(target->directories.begin() + i);
        }
    }

    // Remove files that are out of sync
    for (int i = 0; i < target->files.size(); i++)
    {
        if (target->files[i]->synchronizeIndex != synchronizeIndex)
        {
            DestroyFile(target->files[i]);
            target->files.erase(target->files.begin() + i);
        }
    }

    // Recursively go through all child directories
    for (auto directory : target->directories)
        Synchronize(directory);
}

ProjectDiretory* ProjectAssetsModule::TryFindDirectory(ProjectDiretory* directory, String& path)
{
    for (auto directory : directory->directories)
        if (directory->path == path)
            return directory;
    return nullptr;
}

ProjectFile* ProjectAssetsModule::TryFindFile(ProjectDiretory* directory, String& path)
{
    for (auto file : directory->files)
        if (file->path == path)
            return file;
    return nullptr;
}

Void ProjectAssetsModule::DestroyDirectoryRecursive(ProjectDiretory* target)
{
    for (auto file : target->files)
        DestroyFile(file);

    for (auto directory : target->directories)
        DestroyDirectoryRecursive(directory);

    delete target;
}

Void ProjectAssetsModule::DestroyFile(ProjectFile* target)
{
    delete target;
}*/