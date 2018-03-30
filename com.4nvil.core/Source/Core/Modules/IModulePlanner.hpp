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

#include <functional>
#include <Core\Tools\Common.hpp>
#include <Core\Tools\Collections\List.hpp>

namespace Core
{
    class Module;

    struct ModuleJob
    {
        ModuleJob()
            : module(nullptr)
            , offset(0)
            , size(0)
            , userData(nullptr)
        {
        }
        Module* module;
        uint32 offset;
        size_t size;
        void* userData; // TODO: Maybe we can get rid of it, currently one implementation really needs it for perforamnce
    };

    class IModulePlanner
    {
    public:
        // Recreates the Modules execution plan
        virtual void Recreate(List<Module*>& modules) = 0;

        // Prepares for new frame
        virtual void Reset() = 0;

        // Pulls next executable job, if no job is available nullptr will be returned in Module field
        // Thread-safe
        virtual ModuleJob TryGetNext() = 0;

        // Marks the job as finished
        // Thread-safe
        virtual void SetFinished(const ModuleJob& job) = 0;

    public:
        AUTOMATED_PROPERTY_SET(std::function<void(size_t)>, jobFinishCallback);
        AUTOMATED_PROPERTY_SET(std::function<void(void)>, finishCallback);
    };
}