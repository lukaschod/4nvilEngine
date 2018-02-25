#pragma once

#include <Tools\Math\Math.h>

enum class ColorFormat
{
	RGBA32,
	RGB24,
	RG16,
};

// RGBA
template<class T>
class Color
{
public:
	Color() {}
	Color(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}

public:
	T r, g, b, a;
};

typedef Color<float> Colorf;