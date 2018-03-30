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

#include <Core\Graphics\IGraphicsModule.hpp>
#include <Core\Graphics\IFilter.hpp>
#include <Core\Rendering\SamplerModule.hpp>

using namespace Core;
using namespace Core::Graphics;

void SamplerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Sampler* SamplerModule::AllocateSampler() const
{
    auto filter = graphicsModule->AllocateFilter();
    return new Sampler(filter);
}

SERIALIZE_METHOD_ARG1(SamplerModule, CreateSampler, const Sampler*);

bool SamplerModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateSampler, Sampler*, target);
        target->created = true;
        graphicsModule->RecCreateIFilter(context, target->filter);
        samplers.push_back(target);
        DESERIALIZE_METHOD_END
    }
    return false;
}