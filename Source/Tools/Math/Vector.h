#pragma once

#include <Tools\Math\Math.h>

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

	template<typename T>
	class Vector3
	{
	public:
		inline Vector3() {}
		inline Vector3(T v) : x(v), y(v), z(v) {}
		inline Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
		inline Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z) {}

		inline const Vector3<T>& operator+=(const Vector3<T>& v) { x += v.x; y += v.y; z += v.z; return *this; }
		inline const Vector3<T>& operator-=(const Vector3<T>& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		inline const Vector3<T>& operator*=(const Vector3<T>& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		inline const Vector3<T>& operator/=(const Vector3<T>& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }

		inline const Vector3<T>& operator*=(T v) { x *= v; y *= v; z *= v; return *this; }
		inline const Vector3<T>& operator/=(T v) { x /= v; y /= v; z /= v; return *this; }

		inline bool operator==(const Vector3<T>& v) const { return(x == v.x && y == v.y && z == v.z); }
		inline bool operator!=(const Vector3<T>& v) const { return(x != v.x && y != v.y && z != v.z); }

		inline T SqrMagnitude() const { return x*x + y*y + z*z; }
		inline T Magnitude() const { return Math::Sqrt(SqrMagnitude()); }

		inline void Normalize() { this->operator/=(Magnitude()); }

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


	template<typename T>
	class Vector4
	{
	public:
		inline Vector4() {}
		inline Vector4(T v) : x(v), y(v), z(v), w(v) {}
		inline Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
		inline Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

		inline const Vector4<T>& operator+=(const Vector4<T>& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		inline const Vector4<T>& operator-=(const Vector4<T>& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		inline const Vector4<T>& operator*=(const Vector4<T>& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		inline const Vector4<T>& operator/=(const Vector4<T>& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }

		inline const Vector4<T>& operator*=(T v) { x *= v; y *= v; z *= v; w *= v; return *this; }
		inline const Vector4<T>& operator/=(T v) { x /= v; y /= v; z /= v; w /= v; return *this; }

		inline bool operator==(const Vector4<T>& v) const { return(x == v.x && y == v.y && z == v.z && w == v.w); }
		inline bool operator!=(const Vector4<T>& v) const { return(x != v.x && y != v.y && z != v.z && w != v.w); }

		inline T Dot() { return x*x + y*y + z*z + w*w; }
		inline T Magnitude() { return Math::Sqrt(Dot()); }

		inline static Vector4<T> Max(const Vector4<T>& left, const Vector4<T>& right)
		{
			Vector4<T> out;
			out.x = Math::Max(left.x, right.x);
			out.y = Math::Max(left.y, right.y);
			out.z = Math::Max(left.z, right.z);
			out.w = Math::Max(left.w, right.w);
			return out;
		}

		inline static Vector4<T> Min(const Vector4<T>& left, const Vector4<T>& right)
		{
			Vector4<T> out;
			out.x = Math::Min(left.x, right.x);
			out.y = Math::Min(left.y, right.y);
			out.z = Math::Min(left.z, right.z);
			out.w = Math::Min(left.w, right.w);
			return out;
		}

	public:
		T x, y, z, w;
	};

	typedef Vector4<float> Vector4f;

	template<class T, class Vector>
	inline Vector4<T> operator+(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out += right; return out; }

	template<class T, class Vector>
	inline Vector4<T> operator-(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out -= right; return out; }

	template<class T, class Vector>
	inline Vector4<T> operator/(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out /= right; return out; }

	template<class T, class Vector>
	inline Vector4<T> operator*(const Vector4<T>& left, const Vector& right) { Vector4<T> out(left); out *= right; return out; }

	template<class T, class Vector>
	inline T operator|(const Vector4<T>& left, const Vector& right)
	{
		Vector4<T> out(left);
		out *= right;
		return out.x + out.y + out.z + out.w;
	}

	template<class T>
	inline Vector4<T> operator/(const Vector4<T>& left, T right) { Vector4<T> out(left); out /= right; return out; }

	template<class T>
	inline Vector4<T> operator*(const Vector4<T>& left, T right) { Vector4<T> out(left); out *= right; return out; }

	template<class T>
	inline Vector4<T> operator-(const Vector4<T>& left) { return Vector4<T>(-left.x, -left.y, -left.z, -left.z); }
}