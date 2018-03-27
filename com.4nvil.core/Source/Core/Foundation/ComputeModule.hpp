#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Modules\Module.hpp>
#include <Core\Foundation\ProfilerModule.hpp>

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