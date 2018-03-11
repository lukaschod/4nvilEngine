#pragma once

#include <Core\Tools\Math\Math.h>

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

	typedef Vector2<float> Vector2f;
}