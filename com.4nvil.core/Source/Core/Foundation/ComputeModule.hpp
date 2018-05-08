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
#include <Core/Modules/Module.hpp>
#include <Core/Foundation/ProfilerModule.hpp>

namespace Core
{
    class ComputeModule : public Module
    {
    public:
        BASE_IS(Module);

        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override { profilerModule = ExecuteAfter<ProfilerModule>(moduleManager); }

    protected:
        ProfilerModule* profilerModule;
    };
}