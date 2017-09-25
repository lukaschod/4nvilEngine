#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\ITexture.h>

struct SamplerOptions : public FilterOptions
{
};

struct Sampler
{
	Sampler(const SamplerOptions& options, const IFilter* filter) :
		filter(filter)
	{ }
	const IFilter* filter;
};

class SamplerModule : public CmdModule
{
public:
	SamplerModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Sampler* RecordCreateSampler(const ExecutionContext& context, const SamplerOptions& options);

	inline const Sampler* GetDefaultSampler() const { return samplers[0]; }

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	List<Sampler*> samplers;
	IGraphicsModule* graphicsModule;
};