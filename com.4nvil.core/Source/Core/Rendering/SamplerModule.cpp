#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Graphics\IFilter.h>
#include <Core\Rendering\SamplerModule.h>

using namespace Core;
using namespace Core::Graphics;

void SamplerModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Sampler* SamplerModule::AllocateSampler() const
{
	auto filter = graphicsModule->AllocateFilter();
	return new Sampler(filter);
}

SERIALIZE_METHOD_ARG1(SamplerModule, CreateSampler, const Sampler*);

bool SamplerModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateSampler, Sampler*, target);
		target->created = true;
		graphicsModule->RecCreateIFilter(context, target->filter);
		samplers.push_back(target);
		DESERIALIZE_METHOD_END
	}
	return false;
}