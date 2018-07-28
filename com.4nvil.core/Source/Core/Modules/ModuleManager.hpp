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

#pragma once

#include <Core/Tools/Common.hpp>
#include <Core/Tools/Threading/AutoResetEvent.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Modules/IModuleExecutor.hpp>
#include <Core/Modules/IModulePlanner.hpp>

namespace Core
{
    class Module;

    class ModuleManager
    {
    public:
        CORE_API ModuleManager(IModulePlanner* planner, IModuleExecutor* executor);
        CORE_API ~ModuleManager();

        // Initializes the ModuleManager and other Modules
        CORE_API Void Start();

        // Finalizes ModuleManager and other Modules
        CORE_API Void Stop();

        // Begins new iteration of frame, where all modules will be re-executed at given time
        CORE_API Bool NewFrame();

        // Waits until the current frame finishes
        CORE_API Void WaitForFrame();

        // Adds module to manager, which later on will be used for planning and execution
        CORE_API Void AddModule(Module* module);

        // Find module with specified type
        template<class T> T* GetModule();

        // Find modules with specified type
        template<class T> Void GetModules(List<T*>& out);

        inline const List<Module*>& GetModules() const { return modules; }
        inline UInt32 GetWorkerCount() const { return executor->GetWorkerCount(); }

    public:
        // Request to add new module during runtime
        CORE_API Void RecAddModule(const ExecutionContext& context, Module* module);

        // Request to stop
        CORE_API Void RecStop(const ExecutionContext& context);

    private:
        List<Module*> modules; // Modules contained by the manager
        IModulePlanner* planner; // Planner that will be used for planning Module execution
        IModuleExecutor* executor; // Executor that will be used for Module execution
        Threading::AutoResetEvent sleepEvent;

        Bool requestedStop;
        List<Module*> requestedAddModules;
    };

    template<class T> 
    T* ModuleManager::GetModule()
    {
        for (auto module : modules)
        {
            if (dynamic_cast<T*>(module) != 0)
                return (T*) module;
        }
        ERROR("Can't find specified module");
        return nullptr;
    }

    template<class T>
    Void ModuleManager::GetModules(List<T*>& out)
    {
        for (auto module : modules)
        {
            if (dynamic_cast<T*>(module) != 0)
                out.push_back((T*) module);
        }
    }
}