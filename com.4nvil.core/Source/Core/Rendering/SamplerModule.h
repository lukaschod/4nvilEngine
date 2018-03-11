#pragma once

#include <Core\Tools\Common.h>
#include <Core\Foundation\PipeModule.h>
#include <Core\Graphics\IFilter.h>

namespace Core::Graphics
{
	class IGraphicsModule;
}

namespace Core
{
	struct SamplerOptions : public Graphics::FilterOptions {};

	struct Sampler
	{
		Sampler(const SamplerOptions& options, const Graphics::IFilter* filter) : filter(filter) {}
		const Graphics::IFilter* filter;
	};

	class SamplerModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Sampler* RecCreateSampler(const ExecutionContext& context, const SamplerOptions& options);

		inline const Sampler* GetDefaultSampler() const { return samplers[0]; }

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Sampler*> samplers;
		Graphics::IGraphicsModule* graphicsModule;
	};
}