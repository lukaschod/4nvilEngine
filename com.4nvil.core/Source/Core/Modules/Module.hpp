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

#include <typeinfo>
#include <Core/Tools/Common.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Modules/ModuleManager.hpp>

namespace Core
{
    class Module;

    struct ExecutionContext
    {
        // Index of worker in current execution
        uint32 workerIndex;

        // Start of job offset index
        uint32 start;

        // End of job offset index
        uint32 end;

        // Module that is currently executed
        Module* executingModule;
    };

    class Module
    {
    public:
        BASE_IS(Module);

        // This is where each Module will setup dependencies between other Modules using the ExecuteBefore and ExecuteAfter
        virtual void SetupExecuteOrder(ModuleManager* moduleManager) {}

        // This is where each Module job will be done, context contains additional information about execution
        virtual void Execute(const ExecutionContext& context) {}

        virtual size_t GetExecutionSize() { return 1; }
        virtual size_t GetSplitExecutionSize() { return 1; }
        virtual const char* GetName() { return "Unamed"; }

    protected:
        // Marks that calling module must be executed before the Module. It is used by IModulePlanner to solve dependency trees
        void ExecuteBefore(ModuleManager* moduleManager, Module* module)
        {
            ASSERT(moduleManager != nullptr && module != nullptr);
            module->dependencies.safe_push_back(this);
            module->OnDependancyAdd(moduleManager, this, true);
        }

        // Marks that calling module must be executed before the Module of Type T and returns pointer to it. It is used by IModulePlanner to solve dependency trees
        template<class T> T* ExecuteBefore(ModuleManager* moduleManager)
        {
            ASSERT(moduleManager != nullptr);
            auto module = (Module*) moduleManager->GetModule<T>();
            module->dependencies.safe_push_back(this);
            module->OnDependancyAdd(moduleManager, this, true);
            return (T*) module;
        }

        // Marks that calling module must be executed after the Module. It is used by IModulePlanner to solve dependency trees
        void ExecuteAfter(ModuleManager* moduleManager, Module* module)
        {
            ASSERT(moduleManager != nullptr && module != nullptr);
            dependencies.safe_push_back(module);
            module->OnDependancyAdd(moduleManager, this, false);
        }

        // Marks that calling module must be executed after the Module of type T and returns pointer to it. It is used by IModulePlanner to solve dependency trees
        template<class T> T* ExecuteAfter(ModuleManager* moduleManager)
        {
            ASSERT(moduleManager != nullptr);
            auto module = (Module*) moduleManager->GetModule<T>();
            dependencies.safe_push_back(module);
            module->OnDependancyAdd(moduleManager, this, false);
            return (T*) module;
        }

        virtual void OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore) {}

    private:
        AUTOMATED_PROPERTY_GETADR(List<Module*>, dependencies); // Modules that this current Module depends on
    };
}