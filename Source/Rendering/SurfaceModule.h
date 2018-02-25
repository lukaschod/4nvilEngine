#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>

namespace Core::Graphics
{
	struct IRenderPass;
	class IGraphicsModule;
}

namespace Core
{
	class ImageModule; struct Image;

	struct SurfaceColor
	{
		SurfaceColor() {}
		SurfaceColor(const Image* image)
			: image(image)
			, loadAction(Graphics::LoadAction::Clear)
			, storeAction(Graphics::StoreAction::Store)
			, clearColor(0, 0.2f, 0.4f, 0)
		{
		}

		SurfaceColor(const Image* image, Graphics::LoadAction loadAction, Graphics::StoreAction storeAction)
			: image(image)
			, loadAction(loadAction)
			, storeAction(storeAction)
			, clearColor(0, 0.2f, 0.4f, 0)
		{
		}

		const Image* image;
		Graphics::LoadAction loadAction;
		Graphics::StoreAction storeAction;
		Math::Colorf clearColor;
	};

	struct SurfaceDepth
	{
		const Image* image;
		Graphics::LoadAction loadAction;
		Graphics::StoreAction storeAction;
		float clearDepth;
	};

	struct Surface
	{
		Surface() : renderPass(nullptr) {}
		const Graphics::IRenderPass* renderPass;
		List<SurfaceColor> colors;
		SurfaceDepth depth;
		Math::Rectf viewport;
	};

	class SurfaceModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Surface* RecCreateSurface(const ExecutionContext& context);
		void RecSetColor(const ExecutionContext& context, const Surface* target, uint32 index, const SurfaceColor& color);
		void RecSetDepth(const ExecutionContext& context, const Surface* target, const SurfaceDepth& depth);
		void RecSetViewport(const ExecutionContext& context, const Surface* target, const Graphics::Viewport& viewport);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Surface*> surfaces;
		Graphics::IGraphicsModule* graphicsModule;
	};
}