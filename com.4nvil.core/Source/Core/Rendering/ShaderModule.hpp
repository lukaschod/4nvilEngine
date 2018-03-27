#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Foundation\PipeModule.hpp>
#include <Core\Graphics\Shader.hpp>

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
		BASE_IS(PipeModule);

		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;

	public:
		const Shader* RecCreateShader(const ExecutionContext& context);
		void RecSetShaderPipeline(const ExecutionContext& context, const Shader* target, uint32 index, const Graphics::ShaderPipelineDesc* desc);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Shader*> shaders;
		Graphics::IGraphicsModule* graphicsModule;
	};
}