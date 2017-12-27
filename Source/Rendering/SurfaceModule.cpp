#include <Rendering\SurfaceModule.h>

SurfaceModule::SurfaceModule(uint32_t bufferCount, uint32_t bufferIndexStep) :
	CmdModule(bufferCount, bufferIndexStep)
{
}

void SurfaceModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	ExecuteAfter<ImageModule>(moduleManager);
}

SERIALIZE_METHOD_CREATE(SurfaceModule, Surface);
SERIALIZE_METHOD_ARG3(SurfaceModule, SetColor, const Surface*, uint32_t, const SurfaceColor&);
SERIALIZE_METHOD_ARG2(SurfaceModule, SetDepth, const Surface*, const SurfaceDepth&);
SERIALIZE_METHOD_ARG2(SurfaceModule, SetViewport, const Surface*, const Viewport&);

bool SurfaceModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateSurface, Surface*, target);
		target->renderPass = graphicsModule->RecCreateIRenderPass(context);
		surfaces.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetColor, Surface*, target, uint32_t, index, SurfaceColor, color);
		target->colors.safe_set(index, color);
		ColorAttachment attachment;
		attachment.texture = color.image->texture;
		attachment.loadAction = color.loadAction;
		attachment.storeAction = color.storeAction;
		attachment.clearColor = color.clearColor;
		graphicsModule->RecSetColorAttachment(context, target->renderPass, index, attachment);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetDepth, Surface*, target, SurfaceDepth, depth);
		target->depth = depth;
		DepthAttachment attachment;
		attachment.texture = depth.image->texture;
		attachment.loadAction = depth.loadAction;
		attachment.storeAction = depth.storeAction;
		attachment.clearDepth = depth.clearDepth;
		graphicsModule->RecSetDepthAttachment(context, target->renderPass, attachment);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetViewport, Surface*, target, Viewport, viewport);
		graphicsModule->RecSetViewport(context, target->renderPass, viewport);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
