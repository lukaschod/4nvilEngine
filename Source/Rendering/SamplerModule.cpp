#include <Rendering\SamplerModule.h>

void SamplerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

DECLARE_COMMAND_CODE(CreateSampler);
const Sampler* SamplerModule::RecCreateSampler(const ExecutionContext& context, const SamplerOptions& options)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto filter = graphicsModule->RecCreateIFilter(context, options);
	auto target = new Sampler(options, filter);
	stream.Write(TO_COMMAND_CODE(CreateSampler));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
	return target;
}

bool SamplerModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateSampler, Sampler*, target);
		samplers.push_back(target);
		DESERIALIZE_METHOD_END
	}
	return false;
}