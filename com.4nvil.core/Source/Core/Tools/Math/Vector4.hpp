#pragma once

#include <Core\Tools\Math\Math.hpp>
#include <Core\Tools\Math\Vector3.hpp>

namespace Core::Math
{
	template<typename T>
	class Vector4
	{
	public:
		inline Vector4() {}
		inline Vector4(T v) : x(v), y(v), z(v), w(v) {}
		inline Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
		inline Vector4(const Vector2<T>& v, T z, T w) : x(v.x), y(v.y), z(z), w(w) {}
		inline Vector4(const Vector3<T>& v, T w) : x(v.x), y(v.y), z(v.z), w(w) {}
		inline Vector4(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

		inline const Vector4<T>& operator+=(const Vector4<T>& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		inline const Vector4<T>& operator-=(const Vector4<T>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		inline const Vector4<T>& operator*=(const Vector4<T>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		inline const Vector4<T>& operator/=(const Vector4<T>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
		inline const Vector4<T>& operator+=(T v) { x += v; y += v; z += v; w += v; return *this; }
		inline const Vector4<T>& operator-=(T v) { x -= v; y -= v; z -= v; w -= v; return *this; }
		inline const Vector4<T>& operator*=(T v) { x *= v; y *= v; z *= v; w *= v; return *this; }
		inline const Vector4<T>& operator/=(T v) { x /= v; y /= v; z /= v; w /= v; return *this; }
		inline bool operator==(const Vector4<T>& v) const { return(x == v.x && y == v.y && z == v.z && w == v.w); }
		inline bool operator!=(const Vector4<T>& v) const { return(x != v.x && y != v.y && z != v.z && w != v.w); }

		inline void Normalize();

		// TODO: Temp
		inline Vector3<T> xyz() const { return Vector3<T>(x, y, z); }

		// Vector4 direction is Vector3 with w == 0
		inline bool IsDirection() const { return Math::IsZero(w); }

		// Returns the distance of vector in power of 2
		inline T SqrMagnitude() const { return x*x + y*y + z*z + w*w; }
		
		// Returns the distance of vector
		inline T Magnitude() const { return Math::Sqrt(SqrMagnitude()); }

		// Check if all values are valid for usage (If not nan)
		inline bool IsValid() const { return !isnan(x) && !isnan(y) && !isnan(z) && !isnan(w); }

		// Returns combinatio of both vectors maximums
		inline static Vector4<T> Max(const Vector4<T>& left, const Vector4<T>& right);

		// Returns combination of both vectors minimums
		inline static Vector4<T> Min(const Vector4<T>& left, const Vector4<T>& right);

		// Returns combination of both vectors minimums
		inline static T Dot(const Vector4<T>& left, const Vector4<T>& right);

	public:
		T x, y, z, w;
	};

	typedef Vector4<float> Vector4f;
	typedef Vector4<float> float4;

	template<class T, class Vector>
	inline Vector4<T> operator+(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out += right; return out; }

	template<class T, class Vector>
	inline Vector4<T> operator-(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out -= right; return out; }

	template<class T, class Vector>
	inline Vector4<T> operator/(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out /= right; return out; }

	template<class T, class Vector>
	inline Vector4<T> operator*(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out *= right; return out; }

	template<class T>
	inline Vector4<T> operator+(const Vector4<T>& left, T right) { Vector4<T> out(left); out += right; return out; }

	template<class T>
	inline Vector4<T> operator-(const Vector4<T>& left, T right) { Vector4<T> out(left); out -= right; return out; }

	template<class T>
	inline Vector4<T> operator/(const Vector4<T>& left, T right) { Vector4<T> out(left); out /= right; return out; }

	template<class T>
	inline Vector4<T> operator*(const Vector4<T>& left, T right) { Vector4<T> out(left); out *= right; return out; }

	template<class T>
	inline Vector4<T> operator-(const Vector4<T>& v) { return Vector4<T>(-v.x, -v.y, -v.z, -v.z); }

	template<class T>
	inline void Vector4<T>::Normalize() 
	{
		auto magnitude = Magnitude();
		ASSERT(!Math::IsZero(magnitude));
		this->operator/=(magnitude);
	}

	template<class T>
	inline Vector4<T> Vector4<T>::Max(const Vector4<T>& left, const Vector4<T>& right)
	{
		Vector4<T> out;
		out.x = Math::Max(left.x, right.x);
		out.y = Math::Max(left.y, right.y);
		out.z = Math::Max(left.z, right.z);
		out.w = Math::Max(left.w, right.w);
		return out;
	}

	template<class T>
	inline Vector4<T> Vector4<T>::Min(const Vector4<T>& left, const Vector4<T>& right)
	{
		Vector4<T> out;
		out.x = Math::Min(left.x, right.x);
		out.y = Math::Min(left.y, right.y);
		out.z = Math::Min(left.z, right.z);
		out.w = Math::Min(left.w, right.w);
		return out;
	}

	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& left, const Vector4<T>& right)
	{
		auto combination = left * right;
		return combination.x + combination.y + combination.z + combination.w;
	}
}