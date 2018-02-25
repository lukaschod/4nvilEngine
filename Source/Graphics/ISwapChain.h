#pragma once

#include <Tools\Common.h>

namespace Core
{
	struct IView;
}

namespace Core::Graphics
{
	struct ISwapChain
	{
		ISwapChain(const IView* view) :
			view(view),
			bacBufferCount(2)
		{
		}
		const IView* view;
		const uint32 bacBufferCount;
	};
}