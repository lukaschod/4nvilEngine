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

namespace Core::Math
{
	template<typename T>
	class Rect
	{
	public:
		Rect() {}
		Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}
		inline T GetLeft() const { return x; }
		inline T GetTop() const { return y + height; }
		inline T GetRight() const { return x + width; }
		inline T GetBottom() const { return y; }

	public:
		T x, y, width, height;
	};

	typedef Rect<float> Rectf;
}