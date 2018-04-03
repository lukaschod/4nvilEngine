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

#include <Core/Tools/Math/Math.hpp>
#include <Core/Tools/Math/Vector.hpp>

namespace Core::Math
{
    template<typename T>
    class Quaternion
    {
    public:
        inline Quaternion() {}
        inline Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

        // Get euler rotations from quaternion
        inline Vector3<T> GetEuler() const;

        // Convert euler rotations to quaternion
        inline static Quaternion<T> FromEuler(T pitch, T roll, T yaw);

    public:
        T x, y, z, w;
    };

    typedef Quaternion<float> Quaternionf;

    template<class T>
    inline Quaternion<T> operator-(const Quaternion<T>& left) { return left; } // TODO: Make inversion

    // Taken from vikipedia (https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles)
    template<class T>
    inline Quaternion<T> Quaternion<T>::FromEuler(T pitch, T roll, T yaw)
    {
        Quaternion<T> out;

        // Abbreviations for the various angular functions
        T cy = Math::Cos(yaw * (T)0.5);
        T sy = Math::Sin(yaw * (T)0.5);
        T cr = Math::Cos(roll * (T)0.5);
        T sr = Math::Sin(roll * (T)0.5);
        T cp = Math::Cos(pitch * (T)0.5);
        T sp = Math::Sin(pitch * (T)0.5);

        out.w = cy * cr * cp + sy * sr * sp;
        out.x = cy * sr * cp - sy * cr * sp;
        out.y = cy * cr * sp + sy * sr * cp;
        out.z = sy * cr * cp - cy * sr * sp;
        return out;
    }

    // Taken from vikipedia (https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles)
    template<class T>
    inline Vector3<T> Quaternion<T>::GetEuler() const
    {
        // roll (x-axis rotation)
        T sinr = +2.0 * (w * x + y * z);
        T cosr = +1.0 - 2.0 * (x * x + y * y);
        roll = Math::Atan2(sinr, cosr);

        // pitch (y-axis rotation)
        T sinp = +2.0 * (w * y - z * x);
        if (Math::Abs(sinp) >= 1)
            pitch = Math::CopySign(Math::pi / 2, sinp); // use 90 degrees if out of range
        else
            pitch = Math::Asin(sinp);

        // yaw (z-axis rotation)
        T siny = +2.0 * (w * z + x * y);
        T cosy = +1.0 - 2.0 * (y * y + z * z);
        yaw = Math::Atan2(siny, cosy);

        return Vector3<T>(roll, pitch, yaw);
    }
}