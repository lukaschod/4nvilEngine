#pragma once

#include <Tools\Common.h>
#include <Rendering\RenderingLoopModule.h>

namespace Core::Graphics
{
	class IGraphicsModule;
}

namespace Core
{
	class IViewModule;
}

namespace Core
{
	class StorageModule;
	class MaterialModule;
	class CameraModule;
	class MeshRendererModule;

	class UnlitRenderingLoopModule : public PipeModule
	{
	public:
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) { return true; }

	private:
		CameraModule* cameraModule;
		MeshRendererModule* meshRendererModule;
		Graphics::IGraphicsModule* graphicsModule;
		IViewModule* viewModule;
		MaterialModule* materialModule;
		StorageModule* storageModule;
	};
}