#pragma once

#include <Tools\Math\Math.h>
#include <Tools\Math\Vector2.h>

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