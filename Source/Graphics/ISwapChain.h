#pragma once

#include <Common\EngineCommon.h>

struct IView;

struct ISwapChain
{
	ISwapChain(const IView* view) :
		view(view),
		backBufferCount(2)
	{
	}
	const IView* view;
	const uint32_t backBufferCount;
};