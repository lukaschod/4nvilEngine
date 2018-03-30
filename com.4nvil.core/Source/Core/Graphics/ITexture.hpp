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

#include <Core\Tools\Common.hpp>

namespace Core::Graphics
{
    enum class TextureUsageFlags
    {
        None = 0,
        Render = 1 << 0,
        Shader = 1 << 1,
    };
    IMPLEMENT_ENUM_FLAG(TextureUsageFlags);

    struct ITexture
    {
        ITexture() {}
        ITexture(uint32 width, uint32 height) 
            : width(width)
            , height(height)
            , usage(TextureUsageFlags::Render | TextureUsageFlags::Shader)
        {
        }
        uint32 width;
        uint32 height;
        TextureUsageFlags usage;
    };
}