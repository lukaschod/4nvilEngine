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
#include <Core/Foundation/ComputeModule.hpp>

namespace Core
{
    class TimeModule : public ComputeModule
    {
    public:
        BASE_IS(ComputeModule);

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API virtual Void Execute(const ExecutionContext& context) override;
        inline UInt64 GetDeltaTimeMs() const { return deltaTimeMs; }
        inline Float GetDeltaTime() const { return deltaTime; }

    private:
        StopWatch stopWatch;
        UInt64 deltaTimeMs;
        Float deltaTime;
    };
}