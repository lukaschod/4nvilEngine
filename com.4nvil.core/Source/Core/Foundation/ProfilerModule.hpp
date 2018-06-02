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
#include <Core/Tools/StopWatch.hpp>
#include <Core/Modules/Module.hpp>

namespace Core
{
    struct ProfiledFunction
    {
        ProfiledFunction() {}
        ProfiledFunction(
            const Char* name,
            Float start,
            int parentIndex
        )
            : name(name)
            , start(start)
            , end(start)
            , childFunctionCount(0)
            , parentIndex(parentIndex)
        {
        }
        const Char* name;
        Float start;
        Float end;
        UInt childFunctionCount;
        int parentIndex;
    };

    struct ProfiledWorker
    {
        UInt32 workerIndex;
        List<ProfiledFunction> functions;
    };

    class LogModule;

    class ProfilerModule : public Module
    {
    public:
        BASE_IS(Module);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        virtual Void Execute(const ExecutionContext& context) override;

        Void TraceFunctions();

    public:
        Void RecPushFunction(const ExecutionContext& context, const Char* name);
        Void RecPopFunction(const ExecutionContext& context);

    private:
        LogModule* logModule;
        List<ProfiledWorker> workers;
        StopWatch stopWatch;
        UInt64 frameLeftUntilProfile;
    };

#if !defined(ENABLED_MARK_FUNCTION) && defined(ENABLED_DEBUG)
#   define ENABLED_MARK_FUNCTION
#endif

#ifdef ENABLED_MARK_FUNCTION
#   define MARK_FUNCTION ProfileFunction profileFunction(context, profilerModule, __FUNCTION__)
#else
#   define MARK_FUNCTION (Void) 0
#endif

    struct ProfileFunction
    {
    public:
        ProfileFunction(
            const ExecutionContext& context,
            ProfilerModule* profiler,
            const Char* name)
            : profiler(profiler)
            , context(context)
        {
            profiler->RecPushFunction(context, name);
        }

        ~ProfileFunction()
        {
            profiler->RecPopFunction(context);
        }

    private:
        const ExecutionContext& context;
        ProfilerModule* profiler;
    };
}