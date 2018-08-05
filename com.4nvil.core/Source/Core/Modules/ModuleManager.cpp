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

#include <algorithm>
#include <Core/Modules/ModuleManager.hpp>
#include <Core/Modules/Module.hpp>

using namespace Core;

ModuleManager::ModuleManager(IModulePlanner* planner, IModuleExecutor* executor)
    : planner(planner)
    , executor(executor)
    , requestedStop(false)
{
    ASSERT(planner != nullptr);
    ASSERT(executor != nullptr);

    planner->SetFinishCallback([this]() { sleepEvent.Set(); });
}

ModuleManager::~ModuleManager()
{
}

Void ModuleManager::Start()
{
    for (UInt i = 0; i < modules.size(); i++)
        modules[i]->SetupExecuteOrder(this);
    planner->Recreate(modules);
    executor->Start();
}

Void ModuleManager::Stop()
{
    executor->Stop();
}

Bool ModuleManager::NewFrame()
{
    // TODO: Fix all this mess, currently only made for testing
    if (requestedStop)
        return false;
    for (auto module : requestedAddModules)
    {
        module->SetupExecuteOrder(this);
        modules.push_back(module);
    }
    for (auto module : requestedAddModules)
    {
        RemoveModuleRecursive(module);
    }
    if (!requestedAddModules.empty() || !requestedRemoveModule.empty())
    {
        planner->Recreate(modules);
        requestedAddModules.clear();
        requestedRemoveModule.clear();
    }

    planner->Reset();
    executor->Reset();
    return true;
}

Void ModuleManager::WaitForFrame()
{
    sleepEvent.WaitOne();
}

Void ModuleManager::AddModule(Module* module)
{
    ASSERT(!executor->IsRunning());
    modules.push_back(module);
}

Void ModuleManager::RecAddModule(const ExecutionContext& context, Module* module)
{
    // TODO
    requestedAddModules.push_back(module);
}

Void ModuleManager::RecRemoveModule(const ExecutionContext& context, Module* module)
{
    // TODO
    requestedRemoveModule.push_back(module);
}

Void ModuleManager::RecStop(const ExecutionContext& context)
{
    // TODO
    requestedStop = true;
}

Void ModuleManager::RemoveModuleRecursive(Module* module)
{
    if (modules.contains(module))
        return;

    for (auto other : modules)
    {
        if (other->GetConnections().contains(module))
        {
            RemoveModuleRecursive(other);
            other->Disconnect(this, module);
        }
    }

    modules.remove(module);
    // TODO: Destroy callback
}
