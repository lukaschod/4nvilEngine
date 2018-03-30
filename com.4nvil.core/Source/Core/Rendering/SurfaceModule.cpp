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
#include <Core\Graphics\IRenderPass.hpp>
#include <Core\Rendering\SurfaceModule.hpp>
#include <Core\Rendering\ImageModule.hpp>

using namespace Core;
using namespace Core::Graphics;

void SurfaceModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
    ExecuteAfter<ImageModule>(moduleManager);
}

const Surface* SurfaceModule::AllocateSurface()
{
    auto renderPass = graphicsModule->AllocateRenderPass();
    return new Surface(renderPass);
}

SERIALIZE_METHOD_ARG1(SurfaceModule, CreateSurface, const Surface*);
SERIALIZE_METHOD_ARG3(SurfaceModule, SetColor, const Surface*, uint32, const SurfaceColor&);
SERIALIZE_METHOD_ARG2(SurfaceModule, SetDepth, const Surface*, const SurfaceDepth&);
SERIALIZE_METHOD_ARG2(SurfaceModule, SetViewport, const Surface*, const Viewport&);

bool SurfaceModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(CreateSurface, Surface*, target);
        target->created = true;
        graphicsModule->RecCreateIRenderPass(context, target->renderPass);
        surfaces.push_back(target);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG3_START(SetColor, Surface*, target, uint32, index, SurfaceColor, color);
        ASSERT(target->created);
        target->colors.safe_set(index, color);
        ColorAttachment attachment;
        attachment.texture = color.image->texture;
        attachment.loadAction = color.loadAction;
        attachment.storeAction = color.storeAction;
        attachment.clearColor = color.clearColor;
        graphicsModule->RecSetColorAttachment(context, target->renderPass, index, attachment);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetDepth, Surface*, target, SurfaceDepth, depth);
        ASSERT(target->created);
        target->depth = depth;
        DepthAttachment attachment;
        attachment.texture = depth.image->texture;
        attachment.loadAction = depth.loadAction;
        attachment.storeAction = depth.storeAction;
        attachment.clearDepth = depth.clearDepth;
        graphicsModule->RecSetDepthAttachment(context, target->renderPass, attachment);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG2_START(SetViewport, Surface*, target, Viewport, viewport);
        ASSERT(target->created);
        graphicsModule->RecSetViewport(context, target->renderPass, viewport);
        DESERIALIZE_METHOD_END;
    }
    return false;
}
