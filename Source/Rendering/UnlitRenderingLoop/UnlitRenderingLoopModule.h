#pragma once

#include <Tools\Common.h>
#include <Graphics\IGraphicsModule.h>
#include <Rendering\RenderingLoopModule.h>
#include <Rendering\CameraModule.h>
#include <Rendering\MeshRendererModule.h>
#include <Views\IViewModule.h>

class StorageModule;
class MaterialModule;

class UnlitRenderingLoopModule : public CmdModule
{
public:
	UnlitRenderingLoopModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) { return true; }

private:
	CameraModule* cameraModule;
	MeshRendererModule* meshRendererModule;
	IGraphicsModule* graphicsModule;
	IViewModule* viewModule;
	MaterialModule* materialModule;
	StorageModule* storageModule;
};