#pragma once

#include <Core\Tools\Common.h>
#include <Core\Foundation\PipeModule.h>
#include <Core\Graphics\IRenderPass.h>

namespace Core
{
	struct Image;
	class ImageModule;

	namespace Graphics
	{
		class IGraphicsModule;
	}
}

namespace Core
{
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
		Surface(const Graphics::IRenderPass* renderPass) 
			: renderPass(renderPass)
			, created(false)
		{}
		const Graphics::IRenderPass* renderPass;
		List<SurfaceColor> colors;
		SurfaceDepth depth;
		Math::Rectf viewport;
		bool created;
	};

	class SurfaceModule : public PipeModule
	{
	public:
		BASE_IS(PipeModule);

		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Surface* AllocateSurface();

	public:
		void RecCreateSurface(const ExecutionContext& context, const Surface* surface);
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