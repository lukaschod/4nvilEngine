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

#include <Core\Tools\Math\Matrix.hpp>
#include <Core\Tools\Math\Math.hpp>

using namespace Core;
using namespace Core::Math;

#define MAT(m, r, c) (m)[c*4+r]

float Core::Math::MatrixGetDeterminant(const float* in)
{
	auto m00 = MAT(in, 0, 0);  auto m01 = MAT(in, 0, 1);  auto m02 = MAT(in, 0, 2);  auto m03 = MAT(in, 0, 3);
	auto m10 = MAT(in, 1, 0);  auto m11 = MAT(in, 1, 1);  auto m12 = MAT(in, 1, 2);  auto m13 = MAT(in, 1, 3);
	auto m20 = MAT(in, 2, 0);  auto m21 = MAT(in, 2, 1);  auto m22 = MAT(in, 2, 2);  auto m23 = MAT(in, 2, 3);
	auto m30 = MAT(in, 3, 0);  auto m31 = MAT(in, 3, 1);  auto m32 = MAT(in, 3, 2);  auto m33 = MAT(in, 3, 3);

	auto result =
		m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 - m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
		m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 - m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
		m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 - m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
		m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 - m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
		m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 - m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
		m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 - m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
	return result;
}

bool Core::Math::MatrixInvertGeneral3D(const float* in, float* out)
{
	float pos, neg, t;
	float det;

	// Calculate the determinant of upper left 3x3 sub-matrix and
	// determine if the matrix is singular.
	pos = neg = 0.0;
	t = MAT(in, 0, 0) * MAT(in, 1, 1) * MAT(in, 2, 2);
	if (t >= 0.0)
		pos += t;
	else
		neg += t;

	t = MAT(in, 1, 0) * MAT(in, 2, 1) * MAT(in, 0, 2);
	if (t >= 0.0)
		pos += t;
	else
		neg += t;

	t = MAT(in, 2, 0) * MAT(in, 0, 1) * MAT(in, 1, 2);
	if (t >= 0.0)
		pos += t;
	else
		neg += t;

	t = -MAT(in, 2, 0) * MAT(in, 1, 1) * MAT(in, 0, 2);
	if (t >= 0.0)
		pos += t;
	else
		neg += t;

	t = -MAT(in, 1, 0) * MAT(in, 0, 1) * MAT(in, 2, 2);
	if (t >= 0.0)
		pos += t;
	else
		neg += t;

	t = -MAT(in, 0, 0) * MAT(in, 2, 1) * MAT(in, 1, 2);
	if (t >= 0.0)
		pos += t;
	else
		neg += t;

	det = pos + neg;

	if (det * det < 1e-25)
		return false;

	det = 1.0F / det;
	MAT(out, 0, 0) = ((MAT(in, 1, 1) * MAT(in, 2, 2) - MAT(in, 2, 1) * MAT(in, 1, 2)) * det);
	MAT(out, 0, 1) = (-(MAT(in, 0, 1) * MAT(in, 2, 2) - MAT(in, 2, 1) * MAT(in, 0, 2)) * det);
	MAT(out, 0, 2) = ((MAT(in, 0, 1) * MAT(in, 1, 2) - MAT(in, 1, 1) * MAT(in, 0, 2)) * det);
	MAT(out, 1, 0) = (-(MAT(in, 1, 0) * MAT(in, 2, 2) - MAT(in, 2, 0) * MAT(in, 1, 2)) * det);
	MAT(out, 1, 1) = ((MAT(in, 0, 0) * MAT(in, 2, 2) - MAT(in, 2, 0) * MAT(in, 0, 2)) * det);
	MAT(out, 1, 2) = (-(MAT(in, 0, 0) * MAT(in, 1, 2) - MAT(in, 1, 0) * MAT(in, 0, 2)) * det);
	MAT(out, 2, 0) = ((MAT(in, 1, 0) * MAT(in, 2, 1) - MAT(in, 2, 0) * MAT(in, 1, 1)) * det);
	MAT(out, 2, 1) = (-(MAT(in, 0, 0) * MAT(in, 2, 1) - MAT(in, 2, 0) * MAT(in, 0, 1)) * det);
	MAT(out, 2, 2) = ((MAT(in, 0, 0) * MAT(in, 1, 1) - MAT(in, 1, 0) * MAT(in, 0, 1)) * det);

	// Do the translation part
	MAT(out, 0, 3) = -(MAT(in, 0, 3) * MAT(out, 0, 0) +
		MAT(in, 1, 3) * MAT(out, 0, 1) +
		MAT(in, 2, 3) * MAT(out, 0, 2));
	MAT(out, 1, 3) = -(MAT(in, 0, 3) * MAT(out, 1, 0) +
		MAT(in, 1, 3) * MAT(out, 1, 1) +
		MAT(in, 2, 3) * MAT(out, 1, 2));
	MAT(out, 2, 3) = -(MAT(in, 0, 3) * MAT(out, 2, 0) +
		MAT(in, 1, 3) * MAT(out, 2, 1) +
		MAT(in, 2, 3) * MAT(out, 2, 2));

	MAT(out, 3, 0) = 0.0f;
	MAT(out, 3, 1) = 0.0f;
	MAT(out, 3, 2) = 0.0f;
	MAT(out, 3, 3) = 1.0f;

	return true;
}

#define PP_WRAP_CODE(code) do { code; } while(0)
#define SWAP_ROWS(a, b) PP_WRAP_CODE(float *_tmp = a; (a)=(b); (b)=_tmp;)
bool Core::Math::MatrixInvertFull(const float* m, float* out)
{
	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m, 0, 0); r0[1] = MAT(m, 0, 1);
	r0[2] = MAT(m, 0, 2); r0[3] = MAT(m, 0, 3);
	r0[4] = 1.0; r0[5] = r0[6] = r0[7] = 0.0;

	r1[0] = MAT(m, 1, 0); r1[1] = MAT(m, 1, 1);
	r1[2] = MAT(m, 1, 2); r1[3] = MAT(m, 1, 3);
	r1[5] = 1.0; r1[4] = r1[6] = r1[7] = 0.0;

	r2[0] = MAT(m, 2, 0); r2[1] = MAT(m, 2, 1);
	r2[2] = MAT(m, 2, 2); r2[3] = MAT(m, 2, 3);
	r2[6] = 1.0; r2[4] = r2[5] = r2[7] = 0.0;

	r3[0] = MAT(m, 3, 0); r3[1] = MAT(m, 3, 1);
	r3[2] = MAT(m, 3, 2); r3[3] = MAT(m, 3, 3);
	r3[7] = 1.0; r3[4] = r3[5] = r3[6] = 0.0;

	/* choose pivot - or die */
	if (Math::Abs(r3[0]) > Math::Abs(r2[0]))
		SWAP_ROWS(r3, r2);
	if (Math::Abs(r2[0]) > Math::Abs(r1[0]))
		SWAP_ROWS(r2, r1);
	if (Math::Abs(r1[0]) > Math::Abs(r0[0]))
		SWAP_ROWS(r1, r0);
	if (0.0F == r0[0])
		return false;

	/* eliminate first variable     */
	m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0F)
	{
		r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s;
	}
	s = r0[5];
	if (s != 0.0F)
	{
		r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s;
	}
	s = r0[6];
	if (s != 0.0F)
	{
		r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s;
	}
	s = r0[7];
	if (s != 0.0F)
	{
		r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s;
	}

	/* choose pivot - or die */
	if (Math::Abs(r3[1]) > Math::Abs(r2[1]))
		SWAP_ROWS(r3, r2);
	if (Math::Abs(r2[1]) > Math::Abs(r1[1]))
		SWAP_ROWS(r2, r1);
	if (0.0F == r1[1])
		return false;

	/* eliminate second variable */
	m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0F != s)
	{
		r2[4] -= m2 * s; r3[4] -= m3 * s;
	}
	s = r1[5]; if (0.0F != s)
	{
		r2[5] -= m2 * s; r3[5] -= m3 * s;
	}
	s = r1[6]; if (0.0F != s)
	{
		r2[6] -= m2 * s; r3[6] -= m3 * s;
	}
	s = r1[7]; if (0.0F != s)
	{
		r2[7] -= m2 * s; r3[7] -= m3 * s;
	}

	/* choose pivot - or die */
	if (Math::Abs(r3[2]) > Math::Abs(r2[2]))
		SWAP_ROWS(r3, r2);
	if (0.0F == r2[2])
		return false;

	/* eliminate third variable */
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3]; r3[4] -= m3 * r2[4];
	r3[5] -= m3 * r2[5]; r3[6] -= m3 * r2[6];
	r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0F == r3[3])
		return false;

	s = 1.0F / r3[3];          /* now back substitute row 3 */
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];                /* now back substitute row 2 */
	s = 1.0F / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;

	m1 = r1[2];                /* now back substitute row 1 */
	s = 1.0F / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1); r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1); r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;

	m0 = r0[1];                /* now back substitute row 0 */
	s = 1.0F / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0); r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0); r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out, 0, 0) = r0[4]; MAT(out, 0, 1) = r0[5], MAT(out, 0, 2) = r0[6]; MAT(out, 0, 3) = r0[7];
	MAT(out, 1, 0) = r1[4]; MAT(out, 1, 1) = r1[5], MAT(out, 1, 2) = r1[6]; MAT(out, 1, 3) = r1[7];
	MAT(out, 2, 0) = r2[4]; MAT(out, 2, 1) = r2[5], MAT(out, 2, 2) = r2[6]; MAT(out, 2, 3) = r2[7];
	MAT(out, 3, 0) = r3[4]; MAT(out, 3, 1) = r3[5], MAT(out, 3, 2) = r3[6]; MAT(out, 3, 3) = r3[7];

	return true;
}