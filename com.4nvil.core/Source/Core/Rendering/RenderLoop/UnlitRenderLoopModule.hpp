#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Rendering\RenderLoop\IRenderLoop.hpp>

namespace Core
{
	class StorageModule;
	class MaterialModule;
	class CameraModule;
	class MeshRendererModule;

	namespace Graphics
	{
		struct ISwapChain;
		struct ITexture;
		class IGraphicsModule;
	}
}

namespace Core
{
	class UnlitRenderLoopModule : public IRenderLoopModule
	{
	public:
		BASE_IS(IRenderLoopModule);

		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context);

	public:
		virtual void RecRender(const ExecutionContext& context, const Graphics::ISwapChain* swapChain, const Graphics::ITexture* renderTarget) override;

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;
		void Render(const ExecutionContext& context, const Graphics::ISwapChain* swapChain, const Graphics::ITexture* renderTarget);

	private:
		CameraModule* cameraModule;
		MeshRendererModule* meshRendererModule;
		Graphics::IGraphicsModule* graphicsModule;
		MaterialModule* materialModule;
		StorageModule* storageModule;
	};
}