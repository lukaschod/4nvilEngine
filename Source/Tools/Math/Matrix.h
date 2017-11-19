#pragma once

#include <Tools\Math\Math.h>
#include <Tools\Math\Vector.h>
#include <Tools\Math\Quaternion.h>

// Row-major system
// Left-handed Cartesian Coordinates
template<typename T>
class Matrix4x4
{
public:
	inline Matrix4x4() {}
	inline Matrix4x4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w)
		: x(x), y(y), z(z), w(w) 
	{ }

	inline Vector4<T> Multiply(const Vector4<T>& v)
	{
		Vector4<T> out;
		out.x = x | v;
		out.y = y | v;
		out.z = z | v;
		out.w = w | v;
		out /= out.w;
		return out;
	}

	inline Vector3<T> Multiply(const Vector3<T>& v)
	{
		Vector3<T> out;
		out.x = v.x*x.x + v.y*y.x + v.z*z.x + w.x;
		out.y = v.x*x.y + v.y*y.y + v.z*z.y + w.y;
		out.z = v.x*x.z + v.y*z.z + v.z*z.z + w.z;
		return out;
	}

	inline Matrix4x4<T> Multiply(const Matrix4x4<T>& m)
	{
		Matrix4x4<T> t = Transpose(*this);
		Vector4<T> x_ = m.x, y_ = m.y, z_ = m.z;
		x = Vector4<T>(x_ | t.x, x_ | t.y, x_ | t.z, x_ | t.w);
		y = Vector4<T>(y_ | t.x, y_ | t.y, y_ | t.z, y_ | t.w);
		z = Vector4<T>(z_ | t.x, z_ | t.y, z_ | t.z, z_ | t.w);
		w = Vector4<T>(m.w | t.x, m.w | t.y, m.w | t.z, m.w | t.w);
		return *this;
	}

	template <class T>
	inline static Matrix4x4<T> Indentity()
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(1, 0, 0, 0);
		out.y = Vector4<T>(0, 1, 0, 0);
		out.z = Vector4<T>(0, 0, 1, 0);
		out.w = Vector4<T>(0, 0, 0, 1);
		return out;
	}

	/*inline static Matrix4x4<float> RotateX(float angle)
	{
		Matrix4x4<T> out;
		float c = cos(angle), s = sin(angle);
		out.x = Vector4<float>(1, 0, 0, 0);
		out.y = Vector4<float>(0, c, -s, 0);
		out.z = Vector4<float>(0, s, c, 0);
		out.w = Vector4<float>(0, 0, 0, 1);
		return out;
	}

	inline static Matrix4x4<float> RotateY(float angle)
	{
		Matrix4x4<T> out;
		float c = cos(angle), s = sin(angle);
		out.x = Vector4<float>(c, 0, -s, 0);
		out.y = Vector4<float>(0, 1, 0, 0);
		out.z = Vector4<float>(s, 0, c, 0);
		out.w = Vector4<float>(0, 0, 0, 1);
		return out;
	}

	inline static Matrix4x4<float> RotateZ(float angle)
	{
		Matrix4x4<T> out;
		float c = cos(angle), s = sin(angle);
		out.x = Vector4<float>(c, s, 0, 0);
		out.y = Vector4<float>(-s, c, 0, 0);
		out.z = Vector4<float>(0, 0, 1, 0);
		out.w = Vector4<float>(0, 0, 0, 1);
		return out;
	}*/

	template <class T>
	inline static Matrix4x4<T> Rotate(const Quaternion<T>& rotation)
	{
		// TODO: Make it
		return Indentity<T>();
	}

	template <class T>
	inline static Matrix4x4<T> Scale(const Vector3<T>& scale)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(scale.x, 0, 0, 0);
		out.y = Vector4<T>(0, scale.y, 0, 0);
		out.z = Vector4<T>(0, 0, scale.z, 0);
		out.w = Vector4<T>(0, 0, 0, 1);
		return out;
	}

	template <class T>
	inline static Matrix4x4<T> Translate(const Vector3<T>& translate)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(1, 0, 0, translate.x);
		out.y = Vector4<T>(0, 1, 0, translate.y);
		out.z = Vector4<T>(0, 0, 1, translate.z);
		out.w = Vector4<T>(0, 0, 0, 1);
		return out;
	}

	template <class T>
	inline static Matrix4x4<T> TRS(const Vector3<T>& translate, const Quaternion<T>& rotation, const Vector3<T>& scale)
	{
		Matrix4x4<T> out;
		out = Scale(scale);
		out.Multiply(Rotate(rotation));
		out.Multiply(Translate(translate));
		return out;
	}

	template <class T>
	inline static Matrix4x4<T> Perspective(T aspectRatio, T fieldOfView, T nearClip, T farClip)
	{
		Matrix4x4<T> out;
		const T zDelta = farClip - nearClip;
		const T radians = fieldOfView * (T) 0.5;
		const T cotangent = cos(radians) / sin(radians);
		out.x = Vector4<T>(cotangent / aspectRatio, 0, 0, 0);
		out.y = Vector4<T>(0, cotangent, 0, 0);
		out.z = Vector4<T>(0, 0, farClip / zDelta, -(farClip * nearClip) / zDelta);
		out.w = Vector4<T>(0, 0, 1, 0);
		return out;
	}


	template <class T>
	inline static Matrix4x4<T> Transpose(const Matrix4x4<T>& m)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(m.x.x, m.y.x, m.z.x, m.w.x);
		out.y = Vector4<T>(m.x.y, m.y.y, m.z.y, m.w.y);
		out.z = Vector4<T>(m.x.z, m.y.z, m.z.z, m.w.z);
		out.w = Vector4<T>(m.x.w, m.y.w, m.z.w, m.w.w);
		return out;
	}

public:
	Vector4<T> x, y, z, w;
};

typedef Matrix4x4<float> Matrix4x4f;
