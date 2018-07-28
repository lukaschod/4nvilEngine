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
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Graphics/IRenderPass.hpp>

namespace Core
{
    struct Image;
    class ImageModule;

    namespace Graphics
    {
        class IGraphicsModule;
    }
}

namespace Core
{
    struct SurfaceColor
    {
        SurfaceColor() {}

        SurfaceColor(const Image* image, Graphics::LoadAction loadAction, Graphics::StoreAction storeAction)
            : image(image)
            , loadAction(loadAction)
            , storeAction(storeAction)
            , clearColor(0.19f, 0.3f, 0.47f, 0)
        {
        }

        SurfaceColor(const Image* image) : SurfaceColor(image, Graphics::LoadAction::Clear, Graphics::StoreAction::Store) {}

        const Image* image;
        Graphics::LoadAction loadAction;
        Graphics::StoreAction storeAction;
        Math::Colorf clearColor;
    };

    struct SurfaceDepth
    {
        const Image* image;
        Graphics::LoadAction loadAction;
        Graphics::StoreAction storeAction;
        Float clearDepth;
    };

    struct Surface
    {
        Surface(const Graphics::IRenderPass* renderPass) 
            : renderPass(renderPass)
            , created(false)
        {}
        const Graphics::IRenderPass* renderPass;
        List<SurfaceColor> colors;
        SurfaceDepth depth;
        Math::Rectf viewport;
        Bool created;
    };

    class SurfaceModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;
        CORE_API const Surface* AllocateSurface();

    public:
        CORE_API Void RecCreateSurface(const ExecutionContext& context, const Surface* surface);
        CORE_API Void RecSetColor(const ExecutionContext& context, const Surface* target, UInt32 index, const SurfaceColor& color);
        CORE_API Void RecSetDepth(const ExecutionContext& context, const Surface* target, const SurfaceDepth& depth);
        CORE_API Void RecSetViewport(const ExecutionContext& context, const Surface* target, const Graphics::Viewport& viewport);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Surface*> surfaces;
        Graphics::IGraphicsModule* graphicsModule;
    };
}