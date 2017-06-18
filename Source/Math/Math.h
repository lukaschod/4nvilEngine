#pragma once

#include <Common\EngineCommon.h>

class Math
{
public:
	inline static uint32_t Max(uint32_t first, uint32_t second) { return first > second ? first : second; }
};