#pragma once

#include <Core\Tools\Common.h>
#include <Core\Foundation\PipeModule.h>

namespace Core::Graphics
{
	struct ITexture;
	struct ISwapChain;
}

namespace Core
{
	class IRenderLoopModule : public PipeModule 
	{
	public:
		virtual void RecRender(const ExecutionContext& context, const Graphics::ISwapChain* swapChain, const Graphics::ITexture* renderTarget) = 0;
	};
}