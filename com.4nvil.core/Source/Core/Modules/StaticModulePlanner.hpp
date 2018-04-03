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

#include <vector>
#include <queue>
#include <mutex>
#include <Core/Tools/Common.hpp>
#include <Core/Modules/IModulePlanner.hpp>

namespace Core
{
    struct StaticModulePlanNode
    {
        StaticModulePlanNode(Module* module);
        StaticModulePlanNode* TryFindNode(Module* module);

        List<StaticModulePlanNode*> childs;
        Module* module;
        uint32 concunrency;
        uint32 dependencies;
    };

    class StaticModulePlan
    {
    public:
        StaticModulePlan(List<Module*>& modules);
        inline bool TryAdd(StaticModulePlanNode* node);
        inline StaticModulePlanNode* TryFindNode(Module* module);
        inline void Reset();

        inline StaticModulePlanNode* GetRoot() const { return root; }

    private:
        StaticModulePlanNode* root;
        List<StaticModulePlanNode*> nodes;
    };

    class StaticModulePlanner : public IModulePlanner
    {
    public:
        StaticModulePlanner();
        ~StaticModulePlanner();
        virtual void Recreate(List<Module*>& modules) override;
        virtual void Reset() override;
        virtual ModuleJob TryGetNext() override;
        virtual void SetFinished(const ModuleJob& module) override;

    private:
        void AddJob(StaticModulePlanNode* node);

    private:
        StaticModulePlan* plan;
        std::queue<ModuleJob> readyJobs;
        std::mutex readyModulesMutex;
        size_t jobExecutingCount;
    };
}