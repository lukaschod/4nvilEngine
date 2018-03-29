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
#include <Core\Tools\Math\Vector.hpp>
#include <Core\Tools\Math\Quaternion.hpp>

namespace Core::Math
{
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

		// Transform zero vector position without w normalization
		Vector4<T> GetPosition() const;

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
		static Matrix4x4<T> Orthographic(T left, T right, T bottom, T top, T nearClip, T farClip);
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
		// Pack the operations for SIMD optimization
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
		// Pack the operations for SIMD optimization
		Vector4<T> out;
		out =  x * v.x; // MUL
		out += y * v.y; // MAD
		out += z * v.z; // MAD
		out += w;		// ADD
		return out;
	}

	template<class T>
	inline Vector4<T> Matrix4x4<T>::GetPosition() const
	{
		return w;
	}

	template<class T>
	inline Matrix4x4<T> Matrix4x4<T>::Multiply(const Matrix4x4<T>& m)
	{
		// Pack the operations for SIMD optimization
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
		// Cache for less typing
		T qx = rotation.x;
		T qy = rotation.y;
		T qz = rotation.z;
		T qw = rotation.w;

		Matrix4x4<T> out;
		out.x = Vector4<T>(1 - 2 * qy*qy - 2 * qz*qz, 2 * qx*qy - 2 * qz*qw, 2 * qx*qz + 2 * qy*qw, 0);
		out.y = Vector4<T>(2 * qx*qy + 2 * qz*qw, 1 - 2 * qx*qx - 2 * qz*qz, 2 * qy*qz - 2*qx*qw, 0);
		out.z = Vector4<T>(2 * qx*qz - 2 * qy*qw, 2 * qy*qz + 2 * qx*qw, 1 - 2 * qx*qx - 2 * qy*qy, 0);
		out.w = Vector4<T>(0, 0, 0, 1);
		return out;
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
	inline Matrix4x4<T> Matrix4x4<T>::Orthographic(T left, T right, T bottom, T top, T near, T far)
	{
		Matrix4x4<T> out;
		out.x = Vector4<T>(2 / (right - left), 0, 0, -(right + left) / (right - left));
		out.y = Vector4<T>(0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom));
		out.z = Vector4<T>(0, 0, 2 / (far - near), -(far + near) / (far - near));
		out.w = Vector4<T>(0, 0, 0, 1);
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