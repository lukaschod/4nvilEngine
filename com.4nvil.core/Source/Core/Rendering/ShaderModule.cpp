/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <Core\Graphics\IGraphicsModule.hpp>
#include <Core\Rendering\ShaderModule.hpp>

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
