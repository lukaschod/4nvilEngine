#include <Core\Input\InputModule.h>
#include <Core\Input\ViewInputs.h>
#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Rendering\RenderLoop\IRenderLoop.h>
#include <Core\Rendering\SurfaceModule.h>
#include <Core\Rendering\ImageModule.h>
#include <Editor\Views\ViewLayerModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;
using namespace Editor;

void ViewLayerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	viewModule = ExecuteAfter<IViewModule>(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	surfaceModule = ExecuteBefore<SurfaceModule>(moduleManager);
	imageModule = ExecuteBefore<ImageModule>(moduleManager);
	renderLoopModule = GetRenderLoop(moduleManager);
	view = nullptr;
	swapChain = nullptr;
	surface = nullptr;
	renderTarget = nullptr;
}

DECLARE_COMMAND_CODE(Show);
void ViewLayerModule::RecShow(const ExecutionContext& context, const IView* target)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	surface = surfaceModule->AllocateSurface();
	stream.Write(TO_COMMAND_CODE(Show));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
}

SERIALIZE_METHOD(ViewLayerModule, Hide);

bool ViewLayerModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(Show, const IView*, target);
		view = target;

		// Create swapchain that will be used with view
		swapChain = graphicsModule->AllocateSwapChain(target);
		graphicsModule->RecCreateISwapChain(context, swapChain);

		// Created offscreen render target
		renderTarget = imageModule->AllocateImage(view->width, view->height);
		imageModule->RecCreateImage(context, renderTarget);

		// Create surface that will target offscreen render target
		surfaceModule->RecCreateSurface(context, surface);
		surfaceModule->RecSetColor(context, surface, 0, SurfaceColor(renderTarget, LoadAction::Clear, StoreAction::Store));
		surfaceModule->RecSetViewport(context, surface, Viewport(Rectf(0, 0, 1, 1)));
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_START(Hide);
		// TODO: fix leak
		view = nullptr;
		swapChain = nullptr;
		DESERIALIZE_METHOD_END;
	}
	return false;
}

void ViewLayerModule::Render(const ExecutionContext& context)
{
	renderLoopModule->RecRender(context, swapChain, renderTarget->texture);
}

void ViewLayerModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;

	base::Execute(context);

	if (view == nullptr)
		return;

	auto device = view->viewInputDevice;
	if (device == nullptr)
		return;

	bool render = true;
	bool resize = false;

	auto& stream = device->inputStream;
	auto inputCount = device->inputCount;
	size_t offset = 0;
	for (uint32 i = 0; i < inputCount; i++)
	{
		auto inputType = stream.FastRead<ViewInputType>(offset);
		switch (inputType)
		{
		case ViewInputType::Resize:
		{
			auto& desc = stream.FastRead<ViewInputResizeDesc>(offset);
			resize = true;
			break;
		}

		case ViewInputType::Render:
		{
			render = true;
			break;
		}

		default:
			ERROR("Unknown input type");
		}
	}

	if (render)
		Render(context);
}