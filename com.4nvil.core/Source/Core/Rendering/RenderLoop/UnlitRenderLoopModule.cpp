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
#include <Core/Rendering/RenderLoop/UnlitRenderLoopModule.hpp>
#include <Core/Rendering/MaterialModule.hpp>
#include <Core/Rendering/StorageModule.hpp>
#include <Core/Rendering/CameraModule.hpp>
#include <Core/Rendering/MeshRendererModule.hpp>
#include <Core/Rendering/SurfaceModule.hpp>
#include <Core/Rendering/MeshModule.hpp>
#include <Core/Rendering/ImageModule.hpp>
#include <Core/Views/IViewModule.hpp>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

Void UnlitRenderLoopModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
    base::SetupExecuteOrder(moduleManager);
    cameraModule = ExecuteAfter<CameraModule>(moduleManager);
    meshRendererModule = ExecuteAfter<MeshRendererModule>(moduleManager);
    graphicsModule = ExecuteAfter<IGraphicsModule>(moduleManager);
    materialModule = ExecuteAfter<MaterialModule>(moduleManager);
    storageModule = ExecuteAfter<StorageModule>(moduleManager);
}

Void UnlitRenderLoopModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;
    base::Execute(context);
}

SERIALIZE_METHOD_ARG2(UnlitRenderLoopModule, Render, const ISwapChain*, const ITexture*);

Bool UnlitRenderLoopModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG2_START(Render, const ISwapChain*, swapChain, const ITexture*, renderTarget);
        Render(context, swapChain, renderTarget);
        DESERIALIZE_METHOD_END;
    }
    return false;
}

Void UnlitRenderLoopModule::Render(const ExecutionContext& context, const ISwapChain* swapChain, const ITexture* renderTarget)
{
    // TODO: make it one time only if any view is rendered
    auto& cameras = cameraModule->GetCameras();
    auto& meshRenderers = meshRendererModule->GetMeshRenderers();
    auto perAllRendererStorage = meshRendererModule->GetPerAllRendererStorage();
    graphicsModule->RecPushDebug(context, "UnlitRenderLoopModule::Render");
    for (auto camera : cameras)
    {
        auto surface = camera->surface;
        if (surface == nullptr)
            continue;

        graphicsModule->RecPushDebug(context, "Camera::Render");
        graphicsModule->RecCopyBuffer(context, camera->perCameraStorage->buffer, perAllRendererStorage->buffer, camera->perCameraStorage->size);
        graphicsModule->RecSetRenderPass(context, surface->renderPass);

        for (auto meshRenderer : meshRenderers)
        {
            if (!meshRenderer->unit->activated)
                continue;

            auto mesh = meshRenderer->mesh;
            auto material = meshRenderer->material;
            if (mesh == nullptr || material == nullptr)
                continue;

            auto& pipelines = material->pipelines;
            for (auto pipeline : pipelines)
            {
                DrawDesc draw;
                draw.pipeline = pipeline->pipeline;
                draw.properties = pipeline->properties;
                draw.vertexBuffer = mesh->vertexBuffer;

                for (auto& subMesh : mesh->subMeshes)
                {
                    draw.offset = subMesh.offset;
                    draw.size = subMesh.size;
                    graphicsModule->RecDraw(context, draw);
                }
            }
        }

        graphicsModule->RecPopDebug(context);
    }

    graphicsModule->RecFinalBlit(context, swapChain, renderTarget);
    graphicsModule->RecPresent(context, swapChain, renderTarget);
    graphicsModule->RecPopDebug(context);
}