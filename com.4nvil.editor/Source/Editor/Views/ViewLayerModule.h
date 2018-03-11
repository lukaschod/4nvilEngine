#pragma once

#include <Core\Foundation\PipeModule.h>
#include <Core\Views\IViewModule.h>

using namespace Core;

namespace Core
{
	struct Surface;
	struct Image;
	class SurfaceModule;
	class ImageModule;
	class IRenderLoopModule;
	namespace Graphics
	{
		struct ISwapChain;
		class IGraphicsModule;
	}
}

namespace Editor
{
	class ViewLayerModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override;
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Surface* GetSurface() const { return surface; }

	public:
		void RecShow(const ExecutionContext& context, const IView* view);
		void RecHide(const ExecutionContext& context);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;
		virtual IRenderLoopModule* GetRenderLoop(ModuleManager* moduleManager) = 0;
		virtual void Render(const ExecutionContext& context);

	protected:
		Graphics::IGraphicsModule* graphicsModule;
		SurfaceModule* surfaceModule;
		ImageModule* imageModule;
		IRenderLoopModule* renderLoopModule;
		IViewModule* viewModule;
		const Graphics::ISwapChain* swapChain;
		const Surface* surface;
		const Image* renderTarget;
		const IView* view;
	};
}