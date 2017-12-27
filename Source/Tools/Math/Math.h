#pragma once

#include <Tools\Common.h>
#include <cmath>

template<class T>
struct Range
{
	Range() {}
	Range(const T* pointer, size_t size)
		: pointer(pointer)
		, size(size)
	{
	}

	const T* pointer;
	size_t size;
};

namespace Math
{
	template<typename T>
	inline T Max(T first, T second) { return first > second ? first : second; }

	template<typename T>
	inline T Min(T first, T second) { return first < second ? first : second; }

	template<typename T>
	inline T Sqrt(T v) { return sqrt(v); }

	inline float DegToRad(float v) { return 3.14f * (v / 180.0f); }

	template<typename T>
	inline T Sin(T v) { return sin(v); }

	template<typename T>
	inline T Log2(T v) { return log2(v); }

	template<typename T>
	inline T Pow(T x, T y) { return pow(x, y); }

	template<typename T>
	inline T Pow2(T y) { return 2 << y; }

	template<typename T>
	inline T GetPadded(T value, T padding)
	{
		if (value <= padding)
			return padding;
		else
			return value + value % padding;
	}
};