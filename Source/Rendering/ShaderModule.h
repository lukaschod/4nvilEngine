#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Graphics\Shader.h>

namespace Core::Graphics
{
	struct IFilter;
	class IGraphicsModule;
}

namespace Core
{
	struct Shader
	{
		List<const Graphics::IShaderPipeline*> pipelines;
	};

	class ShaderModule : public PipeModule
	{
	public:
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Shader* RecCreateShader(const ExecutionContext& context);
		void RecSetShaderPipeline(const ExecutionContext& context, const Shader* target, uint32_t index, const Graphics::ShaderPipelineDesc* desc);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Shader*> shaders;
		Graphics::IGraphicsModule* graphicsModule;
	};
}