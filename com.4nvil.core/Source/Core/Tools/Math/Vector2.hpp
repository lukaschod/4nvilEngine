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

#include <Core/Tools/Math/Math.hpp>

namespace Core::Math
{
    template<typename T>
    class Vector2
    {
    public:
        inline Vector2() {}
        inline Vector2(T v) : x(v), y(v) {}
        inline Vector2(T x, T y) : x(x), y(y) {}
        inline Vector2(const Vector2& v) : x(v.x), y(v.y) {}

    public:
        T x, y;
    };

    typedef Vector2<Float> Vector2f;
}