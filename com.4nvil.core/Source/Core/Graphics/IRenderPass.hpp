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
#include <Core/Tools/Math/Color.hpp>
#include <Core/Tools/Math/Rect.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Graphics/ITexture.hpp>

namespace Core::Graphics
{
#define COLOR_ATTACHMENT_MAX_COUNT 4 // TODO: Remove this!!!!!!!!!!!

    enum class StoreAction
    {
        Store,
    };

    enum class LoadAction
    {
        Load,
        Clear,
        DontCare,
    };

    struct ColorAttachment
    {
        ColorAttachment() : ColorAttachment(nullptr, StoreAction::Store, LoadAction::Load) {}
        ColorAttachment(const ITexture* texture) : ColorAttachment(texture, StoreAction::Store, LoadAction::Load) {}
        ColorAttachment(const ITexture* texture, StoreAction storeAction, LoadAction loadAction) :
            texture(texture), storeAction(storeAction), loadAction(loadAction), clearColor(Math::Colorf(0, 0.2f, 0.4f, 0))
        {
        }

        const ITexture* texture;
        StoreAction storeAction;
        LoadAction loadAction;
        Math::Colorf clearColor;
    };

    struct DepthAttachment
    {
        DepthAttachment(const ITexture* texture, StoreAction storeAction, LoadAction loadAction) :
            texture(texture), storeAction(storeAction), loadAction(loadAction), clearDepth(0)
        {
        }
        DepthAttachment() : DepthAttachment(nullptr, StoreAction::Store, LoadAction::Load) {}

        const ITexture* texture;
        StoreAction storeAction;
        LoadAction loadAction;
        Float clearDepth;
    };

    struct Viewport
    {
        Viewport() :
            rect(0, 0, 1, 1),
            nearClipPlane(0.1f),
            farClipPlane(10000)
        {
        }
        Viewport(const Math::Rectf& rect) :
            rect(rect),
            nearClipPlane(0.1f),
            farClipPlane(10000)
        {
        }
        Math::Rectf rect;
        Float nearClipPlane;
        Float farClipPlane;
    };

    struct IRenderPass
    {
        IRenderPass()
        {
            memset(colors, 0, sizeof(ColorAttachment));
        }

        inline Float GetAspect() const
        {
            auto texture = colors[0].texture;
            ASSERT(texture != nullptr);
            return (Float) texture->width / texture->height;
        }

        ColorAttachment colors[COLOR_ATTACHMENT_MAX_COUNT];
        DepthAttachment depth;
        Viewport viewport;
    };
}