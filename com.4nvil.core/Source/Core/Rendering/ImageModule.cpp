#include <Core\Graphics\IGraphicsModule.hpp>
#include <Core\Rendering\ImageModule.hpp>
#include <Core\Rendering\SamplerModule.hpp>

using namespace Core;
using namespace Core::Graphics;

void ImageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	samplerModule = ExecuteBefore<SamplerModule>(moduleManager);
}

const Image* ImageModule::AllocateImage(uint32 width, uint32 height) const
{
	auto texture = graphicsModule->AllocateTexture(width, height);
	return new Image(texture);
}

SERIALIZE_METHOD_ARG1(ImageModule, CreateImage, const Image*);
SERIALIZE_METHOD_ARG2(ImageModule, SetSampler, const Image*, const Sampler*);

bool ImageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateImage, Image*, target);
		graphicsModule->RecCreateITexture(context, target->texture);
		images.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetSampler, Image*, target, const Sampler*, sampler);
		target->sampler = sampler;
		DESERIALIZE_METHOD_END;
	}
	return false;
}