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
#include <Core/Foundation/TimeModule.hpp>

using namespace Core;

TimeModule::TimeModule() {}

void TimeModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    stopWatch.Start();
}

void TimeModule::Execute(const ExecutionContext& context)
{
    stopWatch.Stop();
    deltaTimeMs = stopWatch.GetElapsedMiliseconds();
    deltaTime = deltaTimeMs / 1000.0f;
    stopWatch.Start();
}