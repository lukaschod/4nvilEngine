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

#include <Core/Graphics/IGraphicsModule.hpp>
#include <Core/Rendering/ImageModule.hpp>
#include <Core/Rendering/SamplerModule.hpp>

using namespace Core;
using namespace Core::Graphics;

Void ImageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
    samplerModule = ExecuteBefore<SamplerModule>(moduleManager);
}

const Image* ImageModule::AllocateImage(UInt32 width, UInt32 height) const
{
    auto texture = graphicsModule->AllocateTexture(width, height);
    return new Image(texture);
}

SERIALIZE_METHOD_ARG1(ImageModule, CreateImage, const Image*);
SERIALIZE_METHOD_ARG2(ImageModule, SetSampler, const Image*, const Sampler*);

Bool ImageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateImage, Image*, target);
        graphicsModule->RecCreateITexture(context, target->texture);
        images.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetSampler, Image*, target, const Sampler*, sampler);
        target->sampler = sampler;
        DESERIALIZE_METHOD_END;
    }
    return false;
}