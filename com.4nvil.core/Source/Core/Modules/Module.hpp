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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Modules/ModuleManager.hpp>

namespace Core
{
    class Module
    {
    public:
        BASE_IS(Module);

        // This is where each Module will setup dependencies between other Modules using the ExecuteBefore and ExecuteAfter
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) {}

        // This is where each Module job will be done, context contains additional information about execution
        virtual Void Execute(const ExecutionContext& context) {}

        virtual Bool IsSplittable() const { return false; }
        virtual UInt GetExecutionSize() { return 1; }
        virtual UInt GetSplitExecutionSize() { return 1; }
        virtual const Char* GetName() const { return "Unamed"; }
        inline const List<Module*>& GetDependencies() const { return dependencies; }
        inline const List<Module*>& GetConnections() const { return connections; }

        virtual Void Connect(ModuleManager* moduleManager, Module* module) { connections.push_back(module); }
        virtual Void Disconnect(ModuleManager* moduleManager, Module* module) { connections.remove(module); }

    protected:
        // Marks that calling module must be executed before the Module. It is used by IModulePlanner to solve dependency trees
        virtual Void ExecuteBeforeModule(ModuleManager* moduleManager, Module* module)
        {
            ASSERT(moduleManager != nullptr && module != nullptr);
            module->dependencies.safe_push_back(this);
            module->Connect(moduleManager, this);
        }

        template<class T> Void ExecuteBefore(ModuleManager* moduleManager, List<T*>& modules)
        {
            ASSERT(moduleManager != nullptr);
            moduleManager->GetModules<T>(modules);
            for (Module* module : modules)
                ExecuteBeforeModule(moduleManager, module);
        }

        // Marks that calling module must be executed before the Module of Type T and returns pointer to it. It is used by IModulePlanner to solve dependency trees
        template<class T> T* ExecuteBefore(ModuleManager* moduleManager)
        {
            ASSERT(moduleManager != nullptr);
            auto module = (Module*) moduleManager->GetModule<T>();
            ExecuteBeforeModule(moduleManager, module);
            return (T*) module;
        }

        // Marks that calling module must be executed after the Module. It is used by IModulePlanner to solve dependency trees
        virtual Void ExecuteAfterModule(ModuleManager* moduleManager, Module* module)
        {
            ASSERT(moduleManager != nullptr && module != nullptr);
            dependencies.safe_push_back(module);
            module->Connect(moduleManager, this);
        }

        template<class T> Void ExecuteAfter(ModuleManager* moduleManager, List<T*>& modules)
        {
            ASSERT(moduleManager != nullptr);
            moduleManager->GetModules<T>(modules);
            for (Module* module : modules)
                ExecuteAfterModule(moduleManager, module);
        }

        // Marks that calling module must be executed after the Module of type T and returns pointer to it. It is used by IModulePlanner to solve dependency trees
        template<class T> T* ExecuteAfter(ModuleManager* moduleManager)
        {
            ASSERT(moduleManager != nullptr);
            auto module = (Module*) moduleManager->GetModule<T>();
            ExecuteAfterModule(moduleManager, module);
            return (T*) module;
        }

    private:
        // Execution order dependancies
        List<Module*> dependencies;

        // Existance dependencies
        List<Module*> connections;
    };
}