#include <Rendering\ImageModule.h>

void ImageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
	samplerModule = ExecuteBefore<SamplerModule>(moduleManager);
}

const Image* ImageModule::AllocateImage(uint32_t width, uint32_t height) const
{
	auto texture = graphicsModule->AllocateTexture(width, height);
	return new Image(texture);
}

DECLARE_COMMAND_CODE(CreateImage);
const Image* ImageModule::RecCreateImage(const ExecutionContext& context, uint32_t width, uint32_t height, const Image* image)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = image == nullptr ? AllocateImage(width, height) : image;
	stream.Write(TO_COMMAND_CODE(CreateImage));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG2(ImageModule, SetSampler, const Image*, const Sampler*);

bool ImageModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateImage, Image*, target);
		graphicsModule->RecCreateITexture(context, 0, 0, target->texture);
		images.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetSampler, Image*, target, const Sampler*, sampler);
		target->sampler = sampler;
		DESERIALIZE_METHOD_END;
	}
	return false;
}