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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/Math/Color.hpp>

namespace Core::Graphics
{
    enum class VertexAttributeType
    {
        Position,
        TexCoord0,
    };

    struct VertexAttributeLayout
    {
        VertexAttributeLayout() {}
        VertexAttributeLayout(VertexAttributeType type, Math::ColorFormat format) :
            type(type),
            format(format)
        {
        }

        VertexAttributeType type;
        Math::ColorFormat format;
    };

    struct VertexLayout
    {
        List<VertexAttributeLayout> attributes;
        UInt32 stride;
    };
}