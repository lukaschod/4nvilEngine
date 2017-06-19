#pragma once

#define FEATURE_SIMD

#ifdef FEATURE_SIMD
#define VECLIB_SSE
#include <veclib.h>
#endif

struct Vector4f
{

};