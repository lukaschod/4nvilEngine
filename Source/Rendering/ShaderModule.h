#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>
#include <Rendering\ImageModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\IRenderPass.h>

struct Shader
{
	List<const IShaderPipeline*> pipelines;
};

class ShaderModule : public CmdModule
{
public:
	ShaderModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Shader* RecCreateShader(const ExecutionContext& context);
	void RecSetShaderPipeline(const ExecutionContext& context, const Shader* target, uint32_t index, const ShaderPipelineDesc* desc);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	List<Shader*> shaders;
	IGraphicsModule* graphicsModule;
};