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

#include <Core/Assets/LibraryModule.hpp>
#include <Core/Assets/CrateModule.hpp>

using namespace Core;

// TODO: Technical dept
#ifdef ENABLED_WINDOWS
#include <Core/Tools/Windows/Common.hpp>
#include <psapi.h>
static Void* TryLoadPlatformLibrary(const Char* path)
{
    return LoadLibrary(path);
}
static Void UnloadPlatformLibrary(Void* handle)
{
    FreeLibrary((HMODULE) handle);
}
static Void* FindPlatformLibraryFunction(Void* handle, const Char* functionName)
{
    return GetProcAddress((HMODULE) handle, functionName);
}
#endif

namespace Core
{
    typedef Void(*GetLibraryModules)(List<Module*>&);
    typedef Void(*GetLibraryDependencies)(List<Directory>&);
}

const Library* LibraryModule::AllocateLibrary()
{
    return new Library();
}

Void LibraryModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    crateModule = ExecuteBefore<CrateModule>(moduleManager);
    this->moduleManager = moduleManager;
}

SERIALIZE_METHOD_ARG2(LibraryModule, Load, const Directory&, AsyncCallback<const Library*>&);
SERIALIZE_METHOD_ARG1(LibraryModule, Destroy, const Library*);

Bool LibraryModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG2_START(Load, const Directory, directory, AsyncCallback<const Library*>, callback);
        if (CanLoad(context, directory))
        {
            auto library = const_cast<Library*>(AllocateLibrary());;
            library->directory = directory;
            CHECK(directory.GetWriteTime(library->createTime));

            // Try load, it might fail because of dependencies
            if (Load(context, library))
            {
                libraries.push_back(library);
                callback.IssueCallback(context, (const Library*) library);
            }
            else
            {
                delete library;
                callback.IssueCallback(context, (const Library*) nullptr);
            }
        }
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(Destroy, Library*, library);
        Destroy(context, library);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Bool LibraryModule::Load(const ExecutionContext& context, Library* library)
{
    library->handle = TryLoadPlatformLibrary(library->directory.ToCString());
    if (library->handle == nullptr)
        return false;

    auto getDependencies = (GetLibraryDependencies) FindPlatformLibraryFunction(library->handle, "Core::GetLibraryDependencies");
    if (getDependencies == nullptr)
        return false;

    auto getModules = (GetLibraryModules) FindPlatformLibraryFunction(library->handle, "Core::GetLibraryModules");
    if (getModules == nullptr)
        return false;

    List<Directory> directories;
    getDependencies(directories);

    // Check if all dependencies are now available
    for (auto directory : directories)
    {
        auto dependancy = FindLibrary(directory);
        if (dependancy == nullptr)
            return false;
        library->dependencies.push_back(const_cast<Library*>(dependancy));
    }

    // Get module that will be created
    getModules(library->modules);
    for (auto module : library->modules)
    {
        moduleManager->RecAddModule(context, module);

        // Find all transferers ids
        auto transfererModule = dynamic_cast<TransfererModule*>(module);
        if (transfererModule != nullptr)
            library->transfererIds.push_back(transfererModule->GetTransfererId());
    }

    return true;
}

Bool LibraryModule::CanLoad(const ExecutionContext& context, const Directory& directory)
{
    auto library = const_cast<Library*>(FindLibrary(directory));
    if (library != nullptr)
    {
        DateTime createTime;
        directory.GetWriteTime(createTime);
        if (library->createTime < createTime)
        {
            Destroy(context, library);
            return true;
        }

        return false;
    }
    return true;
}

const Library* LibraryModule::FindLibrary(const Directory& directory) const
{
    for (auto library : libraries)
        if (library->directory == directory)
            return library;
    return nullptr;
}

Void LibraryModule::Destroy(const ExecutionContext& context, Library* library)
{
    // Recursevly destory all libraries that depend on this one
    for (auto other : libraries)
    {
        if (other->dependencies.find(library) != -1)
            Destroy(context, other);
    }

    // Remove all modules that were added by this library
    for (auto module : library->modules)
    {
        moduleManager->RecRemoveModule(context, module);
    }

    // Destroy all crates that depends on this library provided transferers
    for (auto crate : crateModule->GetCrates())
    {
        if (IsConnected(crate, library))
            crateModule->RecDestroy(context, crate);
    }

    libraries.remove(library);
    delete library;
}

Bool LibraryModule::IsConnected(const Crate* crate, Library* library)
{
    for (auto& resource : crate->locals)
    {
        for (auto& transfererId : library->transfererIds)
            if (resource.transfererId == transfererId)
                return true;
    }
    return false;
}
