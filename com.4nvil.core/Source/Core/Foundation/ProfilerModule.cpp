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

#include <Core/Tools/Math/Math.hpp>
#include <Core/Tools/Collections/StringBuilder.hpp>
#include <Core/Foundation/ProfilerModule.hpp>
#include <Core/Foundation/LogModule.hpp>

using namespace Core;

void ProfilerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    logModule = ExecuteBefore<LogModule>(moduleManager);
    workers.resize(moduleManager->GetWorkerCount());
    for (int i = 0; i < workers.size(); i++)
        workers[i].workerIndex = i;
    stopWatch.Restart();
}

void ProfilerModule::Execute(const ExecutionContext& context)
{
    stopWatch.Restart();

    if (frameLeftUntilProfile == 0)
    {
        StringBuilder<1024 * 4> message;
        frameLeftUntilProfile = 120;
        for (auto& worker : workers)
        {
            message.AppendFmt("Worker %d", worker.workerIndex);
            for (auto& function : worker.functions)
                message.AppendFmt(" %s (%.2f)", function.name, function.end - function.start);
            message.Append("\n");
        }
        message.Append("\n");
        logModule->RecWriteFmt(context, "%s", message.ToString());
    }
    else
    {
        frameLeftUntilProfile--;
    }

    // Clear all profiled functions
    for (auto& worker : workers)
        worker.functions.clear();
}

#include <algorithm>
void ProfilerModule::TraceFunctions()
{
    List<ProfiledFunction> functions;
    for (auto& worker : workers)
    {
        functions.push_back_list(worker.functions);
    }

    std::sort(functions.begin(), functions.end(),
        [](const ProfiledFunction& first, const ProfiledFunction& second) { return first.start < second.start; });

    StringBuilder<1024> message;

    static int frame = 0;
    message.AppendFmt("Frame %d ", frame);
    frame++;

    for (auto& function : functions)
    {
        message.AppendFmt(" %s", function.name);
    }
    message.Append("\n");
    TRACE(message.ToString());
}

void ProfilerModule::RecPushFunction(const ExecutionContext& context, const char* name)
{
    auto& worker = workers[context.workerIndex];
    auto& functions = worker.functions;
    auto parentIndex = functions.size() - 1;
    functions.push_back(ProfiledFunction(name, stopWatch.GetElapsedMicroseconds() / 1000.0f, (int) parentIndex));

    // Notice the parent about new child
    if (parentIndex != -1)
        functions[parentIndex].childFunctionCount++;
}

void ProfilerModule::RecPopFunction(const ExecutionContext& context)
{
    auto& worker = workers[context.workerIndex];
    auto& functions = worker.functions;
    auto& function = functions.back();
    function.end = stopWatch.GetElapsedMicroseconds() / 1000.0f;
}
