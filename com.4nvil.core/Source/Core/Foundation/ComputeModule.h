#pragma once

#include <Core\Tools\Common.h>
#include <Core\Modules\Module.h>
#include <Core\Foundation\ProfilerModule.h>

namespace Core
{
	class ComputeModule : public Module
	{
	public:
		BASE_IS(Module);

		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override { profilerModule = ExecuteAfter<ProfilerModule>(moduleManager); }

	protected:
		ProfilerModule* profilerModule;
	};
}