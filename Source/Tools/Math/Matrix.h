#pragma once

#include <Tools\Math\Math.h>
#include <Tools\Math\Vector.h>
#include <Tools\Math\Quaternion.h>

namespace Core::Math
{
	// Row-major system
	// Left-handed Cartesian Coordinates
	template<typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4() {}
		Matrix4x4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w);

		bool operator==(const Matrix4x4<T>& other) const;
		bool operator!=(const Matrix4x4<T>& other) const;

		// Transform position without w normalization
		Vector4<T> TransformPosition(const Vector4<T>& v) const;

		// Transform position without w normalization
		Vector4<T> TransformPosition(const Vector3<T>& v) const;

		Matrix4x4<T> Multiply(const Matrix4x4<T>& m);
		void Transpose();
		T GetDeterminant() const;

		// Returns if all values are finite
		bool IsValid() const;

		static Matrix4x4<T> Rotate(const Quaternion<T>& rotation);
		static Matrix4x4<T> Scale(const Vector3<T>& scale);
		static Matrix4x4<T> Translate(const Vector3<T>& translate);
		static Matrix4x4<T> TRS(const Vector3<T>& translate, const Quaternion<T>& rotation, const Vector3<T>& scale);
		static Matrix4x4<T> Perspective(T aspectRatio, T fieldOfView, T nearClip, T farClip);
		static Matrix4x4<T> Transposed(const Matrix4x4<T>& m);
		static Matrix4x4<T> Inverted(const Matrix4x4<T>& m);

	public:
		const static Matrix4x4<T> indentity;
		const static Matrix4x4<T> zero;

	public:
		Vector4<T> x, y, z, w;
	};

	typedef Matrix4x4<float> Matrix4x4f;
	typedef Matrix4x4<float> float4x4;

	float MatrixGetDeterminant(const float* in);
	bool MatrixInvertGeneral3D(const float* in, float* out);
	bool MatrixInvertFull(const float* m, float* out);

	template<class T>
	inline Matrix4x4<T>::Matrix4x4(const Vector4<T>& x, const Vector4<T>& y, const Vector4<T>& z, const Vector4<T>& w)
		: x(x)
		, y(y)
		, z(z)
		, w(w)
	{
	}

	template<class T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	template<class T>
	inline bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::TransformPosition(const Vector4<T>& v) const
	{
		/*auto transposed = Matrix4x4<T>Transposed(*this); // Trade-off of using column-major matrices
		Vector4<T> out;
		out.x = Vector4<T>::Dot(transposed.x, v);
		out.y = Vector4<T>::Dot(transposed.y, v);
		out.z = Vector4<T>::Dot(transposed.z, v);
		out.w = Vector4<T>::Dot(transposed.w, v);
		return out;*/

		Vector4<T> out;
		out =  x * v.x; // MUL
		out += y * v.y; // MAD
		out += z * v.z; // MAD
		out += w * v.w; // MAD
		return out;
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::TransformPosition(const Vector3<T>& v) const
	{
		/*Vector3<T> out;
		out.x = v.x*x.x + v.y*y.x + v.z*z.x + w.x;
		out.y = v.x*x.y + v.y*y.y + v.z*z.y + w.y;
		out.z = v.x*x.z + v.y*y.z + v.z*z.z + w.z;
		return out;*/

		Vector4<T> out;
		out =  x * v.x; // MUL
		out += y * v.y; // MAD
		out += z * v.z; // MAD
		out += w;		// ADD
		return out;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Multiply(const Matrix4x4<T>& m)
	{
		/*Matrix4x4<T> t = Matrix4x4<T>::Transposed(m);
		Vector4<T> x_ = x, y_ = y, z_ = z;
		x = Vector4<T>(
			Vector4<T>::Dot(x_, t.x),
			Vector4<T>::Dot(x_, t.y),
			Vector4<T>::Dot(x_, t.z),
			Vector4<T>::Dot(x_, t.w));
		y = Vector4<T>(
			Vector4<T>::Dot(y_, t.x),
			Vector4<T>::Dot(y_, t.y),
			Vector4<T>::Dot(y_, t.z),
			Vector4<T>::Dot(y_, t.w));
		z = Vector4<T>(
			Vector4<T>::Dot(z_, t.x),
			Vector4<T>::Dot(z_, t.y),
			Vector4<T>::Dot(z_, t.z),
			Vector4<T>::Dot(z_, t.w));
		w = Vector4<T>(
			Vector4<T>::Dot(w, t.x),
			Vector4<T>::Dot(w, t.y),
			Vector4<T>::Dot(w, t.z),
			Vector4<T>::Dot(w, t.w));*/

		Matrix4x4<T> out;

		out.x =  m.x * x.x; // MUL
		out.x += m.y * x.y; // MAD
		out.x += m.z * x.z; // MAD
		out.x += m.w * x.w; // MAD

		out.y =  m.x * y.x;
		out.y += m.y * y.y; // MAD
		out.y += m.z * y.z; // MAD
		out.y += m.w * y.w; // MAD

		out.z =  m.x * z.x; // MUL
		out.z += m.y * z.y; // MAD
		out.z += m.z * z.z; // MAD
		out.z += m.w * z.w; // MAD

		out.w =  m.x * w.x; // MUL
		out.w += m.y * w.y; // MAD
		out.w += m.z * w.z; // MAD
		out.w += m.w * w.w; // MAD

		*this = out;

		return *this;
	}

	template<class T>
	inline void Matrix4x4<T>::Transpose()
	{
		Math::Swap(x.y, y.x);
		Math::Swap(x.z, z.x);
		Math::Swap(x.w, w.x);
		Math::Swap(y.z, z.y);
		Math::Swap(y.w, w.y);
		Math::Swap(z.w, w.z);
	}

	template<class T>
	inline T Matrix4x4<T>::GetDeterminant() const
	{
		return (T) MatrixGetDeterminant((float*)this);
	}

	template<class T>
	bool Matrix4x4<T>::IsValid() const
	{
		return x.IsValid() && y.IsValid() && z.IsValid() && w.IsValid();
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Rotate(const Quaternion<T>& rotation)
	{
		// TODO: Make it
		return Matrix4x4<T>::indentity;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Scale(const Vector3<T>& scale)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(scale.x, 0, 0, 0);
		out.y = Vector4<T>(0, scale.y, 0, 0);
		out.z = Vector4<T>(0, 0, scale.z, 0);
		out.w = Vector4<T>(0, 0, 0, 1);
		return out;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Translate(const Vector3<T>& translate)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(1, 0, 0, 0);
		out.y = Vector4<T>(0, 1, 0, 0);
		out.z = Vector4<T>(0, 0, 1, 0);
		out.w = Vector4<T>(translate.x, translate.y, translate.z, 1);
		return out;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::TRS(const Vector3<T>& translate, const Quaternion<T>& rotation, const Vector3<T>& scale)
	{
		Matrix4x4<T> out;
		out = Scale(scale);
		out.Multiply(Rotate(rotation));
		out.Multiply(Translate(translate));
		return out;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Perspective(T aspectRatio, T fieldOfView, T nearClip, T farClip)
	{
		Matrix4x4<T> out;
		const T zDelta = farClip - nearClip;
		const T radians = fieldOfView * (T) 0.5;
		const T cotangent = cos(radians) / sin(radians);
		out.x = Vector4<T>(cotangent / aspectRatio, 0, 0, 0);
		out.y = Vector4<T>(0, cotangent, 0, 0);
		out.z = Vector4<T>(0, 0, farClip / zDelta, 1);
		out.w = Vector4<T>(0, 0, -(farClip * nearClip) / zDelta, 0);
		return out;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Transposed(const Matrix4x4<T>& m)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(m.x.x, m.y.x, m.z.x, m.w.x);
		out.y = Vector4<T>(m.x.y, m.y.y, m.z.y, m.w.y);
		out.z = Vector4<T>(m.x.z, m.y.z, m.z.z, m.w.z);
		out.w = Vector4<T>(m.x.w, m.y.w, m.z.w, m.w.w);
		return out;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Inverted(const Matrix4x4<T>& m)
	{
		Matrix4x4<T> out;
		if (!MatrixInvertFull((const float*) &m, (float*) &out))
			return Matrix4x4<T>::zero;
		return out;
	}

	template<class T>
	const Matrix4x4<T> Matrix4x4<T>::indentity(
		Vector4<T>(1, 0, 0, 0),
		Vector4<T>(0, 1, 0, 0),
		Vector4<T>(0, 0, 1, 0),
		Vector4<T>(0, 0, 0, 1));

	template<class T>
	const Matrix4x4<T> Matrix4x4<T>::zero(
		Vector4<T>(0, 0, 0, 0),
		Vector4<T>(0, 0, 0, 0),
		Vector4<T>(0, 0, 0, 0),
		Vector4<T>(0, 0, 0, 0));
}