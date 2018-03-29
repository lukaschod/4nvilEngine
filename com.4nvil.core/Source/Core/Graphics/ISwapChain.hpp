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

#include <Core\Tools\Common.hpp>

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