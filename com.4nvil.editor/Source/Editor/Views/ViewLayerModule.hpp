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

#include <Core/Foundation/PipeModule.hpp>
#include <Core/Views/IViewModule.hpp>

using namespace Core;

namespace Core
{
    struct Surface;
    struct Image;
    class SurfaceModule;
    class ImageModule;
    class IRenderLoopModule;
    namespace Graphics
    {
        struct ISwapChain;
        class IGraphicsModule;
    }
}

namespace Editor
{
    class ViewLayerModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        virtual Void Execute(const ExecutionContext& context) override;
        virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        const Surface* GetSurface() const { return surface; }

    public:
        Void RecShow(const ExecutionContext& context, const IView* view);
        Void RecHide(const ExecutionContext& context);

    protected:
        virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;
        virtual IRenderLoopModule* GetRenderLoop(ModuleManager* moduleManager) pure;
        virtual Void Render(const ExecutionContext& context);

    protected:
        Graphics::IGraphicsModule* graphicsModule;
        SurfaceModule* surfaceModule;
        ImageModule* imageModule;
        IRenderLoopModule* renderLoopModule;
        IViewModule* viewModule;
        const Graphics::ISwapChain* swapChain;
        const Surface* surface;
        const Image* renderTarget;
        const IView* view;
    };
}