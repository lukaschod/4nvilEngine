#include <Rendering\SamplerModule.h>

SamplerModule::SamplerModule(uint32_t bufferCount, uint32_t bufferIndexStep) 
	: CmdModule(bufferCount, bufferIndexStep)
{
}

void SamplerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

DECLARE_COMMAND_CODE(CreateSampler);
const Sampler* SamplerModule::RecCreateSampler(const ExecutionContext& context, const SamplerOptions& options)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto filter = graphicsModule->RecCreateIFilter(context, options);
	auto target = new Sampler(options, filter);
	stream.Write(CommandCodeCreateSampler);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}

bool SamplerModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateSampler, Sampler*, target);
		samplers.push_back(target);
		DESERIALIZE_METHOD_END
	}
	return false;
}