#include <Rendering\ShaderModule.h>

ShaderModule::ShaderModule(uint32_t bufferCount, uint32_t bufferIndexStep) :
	CmdModule(bufferCount, bufferIndexStep)
{
}

void ShaderModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

SERIALIZE_METHOD_CREATE(ShaderModule, Shader);
SERIALIZE_METHOD_ARG3(ShaderModule, SetShaderPipeline, const Shader*, uint32_t, const ShaderPipelineDesc*);

bool ShaderModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateShader, Shader*, target);
		shaders.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetShaderPipeline, Shader*, target, uint32_t, index, const ShaderPipelineDesc*, desc);
		auto pipeline = graphicsModule->RecCreateIShaderPipeline(context, desc);
		target->pipelines.push_back(pipeline);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
