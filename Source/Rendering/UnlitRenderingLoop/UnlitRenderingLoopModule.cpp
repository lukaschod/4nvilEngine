#include <Rendering\UnlitRenderingLoop\UnlitRenderingLoopModule.h>
#include <Rendering\MaterialModule.h>
#include <Rendering\StorageModule.h>

void UnlitRenderingLoopModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	cameraModule = ExecuteAfter<CameraModule>(moduleManager);
	meshRendererModule = ExecuteAfter<MeshRendererModule>(moduleManager);
	graphicsModule = ExecuteAfter<IGraphicsModule>(moduleManager);
	viewModule = ExecuteAfter<IViewModule>(moduleManager);
	materialModule = ExecuteAfter<MaterialModule>(moduleManager);
	storageModule = ExecuteAfter<StorageModule>(moduleManager);
}

void UnlitRenderingLoopModule::Execute(const ExecutionContext& context)
{
	PROFILE_FUNCTION;
	EXT_TRACE("UnlitRenderingLoopModule::Execute");
	PipeModule::Execute(context);

	auto& cameras = cameraModule->GetCameras();
	auto& meshRenderers = meshRendererModule->GetMeshRenderers();
	auto perAllRendererStorage = meshRendererModule->GetPerAllRendererStorage();
	for (auto camera : cameras)
	{
		auto surface = camera->surface;
		if (surface == nullptr)
			continue;

		storageModule->RecUpdateStorage(context, perAllRendererStorage, 0, Range<void>(&camera->worldToCameraMatrix, sizeof(Matrix4x4f)));

		graphicsModule->RecPushDebug(context, "Camera.Render");
		graphicsModule->RecSetRenderPass(context, surface->renderPass);

		for (auto meshRenderer : meshRenderers)
		{
			auto mesh = meshRenderer->mesh;
			auto material = meshRenderer->material;
			if (mesh == nullptr || material == nullptr)
				continue;

			auto& pipelines = material->pipelines;
			for (auto pipeline : pipelines)
			{
				DrawDesc draw;
				draw.pipeline = pipeline->pipeline;
				draw.properties = pipeline->properties;
				draw.vertexBuffer = mesh->vertexBuffer;
				
				//graphicsModule->RecSetBuffer(context, draw.properties, "_perCameraData", camera->perCameraStorage->buffer);
				graphicsModule->RecSetBuffer(context, draw.properties, "_perMeshData", meshRenderer->perMeshStorage->buffer);
				
				for (auto& subMesh : mesh->subMeshes)
				{
					draw.offset = subMesh.offset;
					draw.size = subMesh.size;
					graphicsModule->RecDraw(context, draw);
				}
			}
		}

		graphicsModule->RecPopDebug(context);
	}

	auto& views = viewModule->GetViews();
	for (auto view : views)
	{
		graphicsModule->RecFinalBlit(context, view->swapChain, view->renderTarget->texture);
	}

	for (auto view : views)
	{
		graphicsModule->RecPresent(context, view->swapChain, view->renderTarget->texture);
	}
}