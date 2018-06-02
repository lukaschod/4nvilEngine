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

    planner->SetFinishCallback([this]()
    {
        sleepEvent.Set();
    });
}

ModuleManager::~ModuleManager()
{
    for (auto module : modules)
        delete module;
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

Void ModuleManager::NewFrame()
{
    planner->Reset();
    executor->Reset();
}

Void ModuleManager::WaitForFrame()
{
    sleepEvent.WaitOne();
}

Void ModuleManager::AddModule(Module* module)
{
    modules.push_back(module);
}