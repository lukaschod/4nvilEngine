#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Rendering\ImageModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\IRenderPass.h>

struct Shader
{
	List<const IShaderPipeline*> pipelines;
};

class ShaderModule : public PipeModule
{
public:
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Shader* RecCreateShader(const ExecutionContext& context);
	void RecSetShaderPipeline(const ExecutionContext& context, const Shader* target, uint32_t index, const ShaderPipelineDesc* desc);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode) override;

private:
	List<Shader*> shaders;
	IGraphicsModule* graphicsModule;
};