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

#include <Core\Tools\Math\Vector.hpp>

namespace Core
{
    struct IView;

    enum class MouseInputType
    {
        Button,
        Move,
    };

    enum class MouseButtonType
    {
        Left,
        Right,
        Center,
        Count,
    };

    struct MouseButtonState
    {
        bool up;
        bool down;
        bool click;
    };

    struct MouseButtonDesc
    {
        MouseButtonType type;
        bool isDown;
    };

    struct MousePositionDesc
    {
        Math::Vector2f position;
        const IView* onView;
    };
}