#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.h>
#include <Core\Rendering\MaterialModule.h>
#include <Core\Rendering\StorageModule.h>
#include <Core\Rendering\CameraModule.h>
#include <Core\Rendering\MeshRendererModule.h>
#include <Core\Rendering\SurfaceModule.h>
#include <Core\Rendering\MeshModule.h>
#include <Core\Rendering\ImageModule.h>
#include <Core\Views\IViewModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

void UnlitRenderLoopModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	cameraModule = ExecuteAfter<CameraModule>(moduleManager);
	meshRendererModule = ExecuteAfter<MeshRendererModule>(moduleManager);
	graphicsModule = ExecuteAfter<IGraphicsModule>(moduleManager);
	materialModule = ExecuteAfter<MaterialModule>(moduleManager);
	storageModule = ExecuteAfter<StorageModule>(moduleManager);
}

SERIALIZE_METHOD_ARG2(UnlitRenderLoopModule, Render, const ISwapChain*, const ITexture*);

void UnlitRenderLoopModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;
	base::Execute(context);
}

bool UnlitRenderLoopModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG2_START(Render, const ISwapChain*, swapChain, const ITexture*, renderTarget);
		Render(context, swapChain, renderTarget);
		DESERIALIZE_METHOD_END;
	}
	return false;
}

void UnlitRenderLoopModule::Render(const ExecutionContext& context, const ISwapChain* swapChain, const ITexture* renderTarget)
{
	// TODO: make it one time only if any view is rendered
	auto& cameras = cameraModule->GetCameras();
	auto& meshRenderers = meshRendererModule->GetMeshRenderers();
	auto perAllRendererStorage = meshRendererModule->GetPerAllRendererStorage();
	for (auto camera : cameras)
	{
		auto surface = camera->surface;
		if (surface == nullptr)
			continue;

		//storageModule->RecUpdateStorage(context, perAllRendererStorage, 0, Range<void>(&camera->worldToCameraMatrix, sizeof(Matrix4x4f)));
		storageModule->RecCopyStorage(context, camera->perCameraStorage, perAllRendererStorage, camera->perCameraStorage->size);

		graphicsModule->RecPushDebug(context, "Camera::Render");
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

	graphicsModule->RecFinalBlit(context, swapChain, renderTarget);
	graphicsModule->RecPresent(context, swapChain, renderTarget);
}