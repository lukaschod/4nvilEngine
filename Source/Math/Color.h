#pragma once

#include <Math\Math.h>

enum ColorFormat
{
	ColorFormatR32G32B32A32,
	ColorFormatR32G32B32,
	ColorFormatR32G32,
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