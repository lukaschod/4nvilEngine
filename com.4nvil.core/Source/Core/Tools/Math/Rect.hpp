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