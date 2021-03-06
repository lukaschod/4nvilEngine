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

/*#pragma once

#include <Core/Tools/String.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/PipeModule.hpp>

using namespace Core;

namespace Editor
{
    struct ProjectFile
    {
        String path;
        UInt64 synchronizeIndex;
    };

    struct ProjectDiretory
    {
        String path;
        UInt64 synchronizeIndex;
        List<ProjectDiretory*> directories;
        List<ProjectFile*> files;
    };

    class ProjectAssetsModule : public PipeModule
    {
    public:
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        // Synchronize directories and files with the physical directories and files
        Void RecSynchronize(const ExecutionContext& context);

        // Set the main path of project files
        Void RecSetPath(const ExecutionContext& context, String& path);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        Void Synchronize(ProjectDiretory* directory);

        // Find directory, not recursive
        ProjectDiretory* TryFindDirectory(ProjectDiretory* directory, String& path);

        // Find file, not recursive
        ProjectFile* TryFindFile(ProjectDiretory* directory, String& path);

        Void DestroyDirectoryRecursive(ProjectDiretory* directory);

        Void DestroyFile(ProjectFile* directory);

    private:
        ProjectDiretory mainDirectory;
        UInt64 synchronizeIndex;
    };
}*/