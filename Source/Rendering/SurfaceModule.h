#pragma once

#include <Tools\Common.h>
#include <Modules\CmdModule.h>
#include <Rendering\ImageModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\IRenderPass.h>

struct SurfaceColor
{
	SurfaceColor() {}
	SurfaceColor(const Image* image) :
		image(image),
		loadAction(LoadActionClear),
		storeAction(StoreActionStore),
		clearColor(0, 0.2f, 0.4f, 0)
	{
	}

	SurfaceColor(const Image* image, LoadAction loadAction, StoreAction storeAction) :
		image(image),
		loadAction(loadAction),
		storeAction(storeAction),
		clearColor(0, 0.2f, 0.4f, 0)
	{
	}
	
	const Image* image;
	LoadAction loadAction;
	StoreAction storeAction;
	Colorf clearColor;
};

struct SurfaceDepth
{
	const Image* image;
	LoadAction loadAction;
	StoreAction storeAction;
	float clearDepth;
};

struct Surface
{
	Surface() : renderPass(nullptr) {}
	const IRenderPass* renderPass;
	List<SurfaceColor> colors;
	SurfaceDepth depth;
	Rectf viewport;
};

class SurfaceModule : public CmdModule
{
public:
	SurfaceModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Surface* RecCreateSurface(const ExecutionContext& context);
	void RecSetColor(const ExecutionContext& context, const Surface* target, uint32_t index, const SurfaceColor& color);
	void RecSetDepth(const ExecutionContext& context, const Surface* target, const SurfaceDepth& depth);
	void RecSetViewport(const ExecutionContext& context, const Surface* target, const Viewport& viewport);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	List<Surface*> surfaces;
	IGraphicsModule* graphicsModule;
};