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
	struct Sampler
	{
		Sampler(const Graphics::IFilter* filter) 
			: filter(filter)
			, created(false)
		{}
		const Graphics::IFilter* filter;
		bool created;
	};

	class SamplerModule : public PipeModule
	{
	public:
		BASE_IS(PipeModule);

		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Sampler* AllocateSampler() const;

		inline const Sampler* GetDefaultSampler() const { return samplers[0]; }

	public:
		void RecCreateSampler(const ExecutionContext& context, const Sampler* target);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Sampler*> samplers;
		Graphics::IGraphicsModule* graphicsModule;
	};
}