#pragma once

#include <Tools\Common.h>
#include <Tools\String.h>
#include <Modules\CmdModule.h>
#include <Rendering\ImageModule.h>
#include <Tools\Math\Rect.h>

struct IView
{
	IView(const Image* renderTarget)
		: renderTarget(renderTarget)
	{ }
	const Image* renderTarget;
	const ISwapChain* swapChain;
	uint32_t width;
	uint32_t height;
};

class IViewModule : public CmdModule
{
public:
	IViewModule(uint32_t bufferCount, uint32_t workersCount) : CmdModule(bufferCount, workersCount) {}
	virtual const IView* RecCreateIView(const ExecutionContext& context, const IView* view = nullptr) = 0;
	virtual const List<const IView*>& GetViews() = 0;
};