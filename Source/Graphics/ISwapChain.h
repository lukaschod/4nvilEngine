#pragma once

#include <Common\EngineCommon.h>

struct IView;

struct ISwapChain
{
	ISwapChain(const IView* view) :
		view(view),
		bacBufferCount(2)
	{
	}
	const IView* view;
	const uint32_t bacBufferCount;
};