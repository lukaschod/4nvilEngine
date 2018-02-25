#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\ITexture.h>

struct SamplerOptions : public FilterOptions
{
};

struct Sampler
{
	Sampler(const SamplerOptions& options, const IFilter* filter)
		: filter(filter)
	{ }
	const IFilter* filter;
};

class SamplerModule : public PipeModule
{
public:
	virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Sampler* RecCreateSampler(const ExecutionContext& context, const SamplerOptions& options);

	inline const Sampler* GetDefaultSampler() const { return samplers[0]; }

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode) override;

private:
	List<Sampler*> samplers;
	IGraphicsModule* graphicsModule;
};