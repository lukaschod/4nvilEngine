#pragma once

#include <Tools\Math\Math.h>

template<typename T>
class Quaternion
{
public:
	inline Quaternion() {}
	inline Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

public:
	T x, y, z, w;
};

typedef Quaternion<float> Quaternionf;

template<class T>
inline Quaternion<T> operator-(const Quaternion<T>& left) { return left; } // TODO: Make inversion