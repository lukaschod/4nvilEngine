#pragma once

#include <cmath>
#include <Tools\Common.h>

namespace Core
{
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
}

namespace Core::Math
{
	const float epsilon = 1e-5f;

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

	template<typename T>
	inline T Clamp(T value, T min, T max) { return Max(Min(value, max), min); }

	template<typename T>
	inline T Abs(T value)
	{
		return abs(value);
	}

	template<typename T>
	inline void Swap(T& first, T& second)
	{
		T temp = first;
		first = second;
		second = temp;
	}

	inline bool IsZero(float value)
	{
		return Math::Abs(value) < epsilon;
	}

	inline size_t SplitJobs(size_t total, size_t pieces, size_t minimum = 1)
	{
		return total > minimum ? (total / pieces) : total;
	}
};