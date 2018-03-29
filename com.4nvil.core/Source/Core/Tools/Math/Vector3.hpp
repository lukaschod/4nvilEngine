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

#include <Core\Tools\Math\Math.hpp>
#include <Core\Tools\Math\Vector2.hpp>

namespace Core::Math
{
	template<typename T>
	class Vector3
	{
	public:
		inline Vector3() {}
		inline Vector3(T v) : x(v), y(v), z(v) {}
		inline Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
		inline Vector3(const Vector2<T>& v, T z) : x(v.x), y(v.y), z(z) {}
		inline Vector3(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z) {}

		inline const Vector3<T>& operator+=(const Vector3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		inline const Vector3<T>& operator-=(const Vector3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		inline const Vector3<T>& operator*=(const Vector3<T>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		inline const Vector3<T>& operator/=(const Vector3<T>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		inline const Vector3<T>& operator*=(T v) { x *= v; y *= v; z *= v; return *this; }
		inline const Vector3<T>& operator/=(T v) { x /= v; y /= v; z /= v; return *this; }
		inline bool operator==(const Vector3<T>& v) const { return(x == v.x && y == v.y && z == v.z); }
		inline bool operator!=(const Vector3<T>& v) const { return(x != v.x && y != v.y && z != v.z); }

		inline void Normalize();

		// Returns the distance of vector in power of 2
		inline T SqrMagnitude() const { return x*x + y*y + z*z; }

		// Returns the distance of vector
		inline T Magnitude() const { return Math::Sqrt(SqrMagnitude()); }

		// Returns if all values are finite
		inline bool IsValid() const { return !isfinite(x) && !isfinite(y) && !isfinite(z); }

	public:
		const static Vector3<T> zero;
		const static Vector3<T> one;

	public:
		T x, y, z;
	};

	typedef Vector3<float> Vector3f;

	template<class T>
	inline Vector3<T> operator-(const Vector3<T>& left) { return Vector3<T>(-left.x, -left.y, -left.z); }

	template<class T, class Other>
	inline Vector3<T> operator-(const Vector3<T>& left, const Other& right) { Vector3<T> out(left); out -= right; return out; }

	template<class T, class Other>
	inline Vector3<T> operator+(const Vector3<T>& left, const Other& right) { Vector3<T> out(left); out += right; return out; }

	template<class T, class Other>
	inline Vector3<T> operator*(const Vector3<T>& left, const Other& right) { Vector3<T> out(left); out *= right; return out; }

	template<class T, class Other>
	inline Vector3<T> operator/(const Vector3<T>& left, const Other& right) { Vector3<T> out(left); out /= right; return out; }

	template<class T>
	inline void Vector3<T>::Normalize()
	{
		auto magnitude = Magnitude();
		ASSERT(!Math::IsZero(magnitude));
		this->operator/=(magnitude);
	}

	template<class T> const Vector3<T> Vector3<T>::zero = Vector3<T>(0, 0, 0);
	template<class T> const Vector3<T> Vector3<T>::one = Vector3<T>(1, 1, 1);
}