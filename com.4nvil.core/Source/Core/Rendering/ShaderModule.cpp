#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Rendering\ShaderModule.h>

using namespace Core;
using namespace Core::Graphics;

void ShaderModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

SERIALIZE_METHOD_CREATE(ShaderModule, Shader);
SERIALIZE_METHOD_ARG3(ShaderModule, SetShaderPipeline, const Shader*, uint32, const ShaderPipelineDesc*);

bool ShaderModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateShader, Shader*, target);
		shaders.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(SetShaderPipeline, Shader*, target, uint32, index, const ShaderPipelineDesc*, desc);
		auto pipeline = graphicsModule->RecCreateIShaderPipeline(context, desc);
		target->pipelines.push_back(pipeline);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
