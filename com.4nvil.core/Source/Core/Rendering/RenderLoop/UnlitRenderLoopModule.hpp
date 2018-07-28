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
#include <Core/Rendering/RenderLoop/IRenderLoop.hpp>

namespace Core
{
    class StorageModule;
    class MaterialModule;
    class CameraModule;
    class MeshRendererModule;

    namespace Graphics
    {
        struct ISwapChain;
        struct ITexture;
        class IGraphicsModule;
    }
}

namespace Core
{
    class UnlitRenderLoopModule : public IRenderLoopModule
    {
    public:
        BASE_IS(IRenderLoopModule);

        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API virtual Void Execute(const ExecutionContext& context);

    public:
        CORE_API virtual Void RecRender(const ExecutionContext& context, const Graphics::ISwapChain* swapChain, const Graphics::ITexture* renderTarget) override;

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;
        Void Render(const ExecutionContext& context, const Graphics::ISwapChain* swapChain, const Graphics::ITexture* renderTarget);

    private:
        CameraModule* cameraModule;
        MeshRendererModule* meshRendererModule;
        Graphics::IGraphicsModule* graphicsModule;
        MaterialModule* materialModule;
        StorageModule* storageModule;
    };
}