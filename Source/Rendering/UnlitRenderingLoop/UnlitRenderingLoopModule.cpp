#include <Rendering\UnlitRenderingLoop\UnlitRenderingLoopModule.h>
#include <Rendering\MaterialModule.h>

UnlitRenderingLoopModule::UnlitRenderingLoopModule(uint32_t bufferCount, uint32_t workersCount) :
	CmdModule(bufferCount, workersCount)
{
}

void UnlitRenderingLoopModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	cameraModule = ExecuteAfter<CameraModule>(moduleManager);
	meshRendererModule = ExecuteAfter<MeshRendererModule>(moduleManager);
	graphicsModule = ExecuteAfter<IGraphicsModule>(moduleManager);
	viewModule = ExecuteAfter<IViewModule>(moduleManager);
	materialModule = ExecuteAfter<MaterialModule>(moduleManager);
}

void UnlitRenderingLoopModule::Execute(const ExecutionContext& context)
{
	EXT_TRACE("UnlitRenderingLoopModule::Execute");
	CmdModule::Execute(context);

	auto& cameras = cameraModule->GetCameras();
	auto& meshRenderers = meshRendererModule->GetMeshRenderers();
	for (auto camera : cameras)
	{
		auto surface = camera->surface;
		if (surface == nullptr)
			continue;

		graphicsModule->RecordPushDebug(context, "Camera.Render");
		graphicsModule->RecordSetRenderPass(context, surface->renderPass);

		for (auto meshRenderer : meshRenderers)
		{
			auto mesh = meshRenderer->mesh;
			auto material = meshRenderer->material;
			if (mesh == nullptr || material == nullptr)
				continue;

			auto pipelines = material->pipelines;
			for (auto pipeline : pipelines)
			{
				DrawSimple draw;
				draw.pipeline = pipeline->pipeline;
				draw.properties = pipeline->properties;
				draw.vertexBuffer = mesh->vertexBuffer;
				
				graphicsModule->RecordSetBuffer(context, draw.properties, "_PerCameraData", camera->perCameraStorage->buffer);
				graphicsModule->RecordSetBuffer(context, draw.properties, "_PerMeshData", meshRenderer->perMeshStorage->buffer);
				
				for (auto subMesh : mesh->subMeshes)
				{
					draw.offset = subMesh.offset;
					draw.size = subMesh.size;
					graphicsModule->RecordBindDrawSimple(context, draw);
				}
			}
		}

		graphicsModule->RecordPopDebug(context);
	}

	auto& views = viewModule->GetViews();
	for (auto view : views)
	{
		graphicsModule->RecordPresent(context, view->swapChain, view->renderTarget->texture);
	}
}