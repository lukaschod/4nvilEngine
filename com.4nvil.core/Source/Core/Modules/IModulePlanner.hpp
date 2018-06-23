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
#include <Core/Tools/Common.hpp>
#include <Core/Tools/Collections/List.hpp>

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
        UInt32 offset;
        UInt size;
        Void* userData; // TODO: Maybe we can get rid of it, currently one implementation really needs it for perforamnce
    };

    class IModulePlanner
    {
    public:
        // Recreates the Modules execution plan
        virtual Void Recreate(List<Module*>& modules) pure;

        // Prepares for new frame
        virtual Void Reset() pure;

        // Pulls next executable job, if no job is available nullptr will be returned in Module field
        // Thread-safe
        virtual ModuleJob TryGetNext() pure;

        // Marks the job as finished
        // Thread-safe
        virtual Void SetFinished(const ModuleJob& job) pure;

        // Set callback for job finish
        inline Void SetJobFinishCallback(std::function<Void(UInt)> callback) { jobFinishCallback = callback; }

        // Set callback for finish
        inline Void SetFinishCallback(std::function<Void(Void)> callback) { finishCallback = callback; }

    public:
        std::function<Void(UInt)> jobFinishCallback;
        std::function<Void(Void)> finishCallback;
    };
}