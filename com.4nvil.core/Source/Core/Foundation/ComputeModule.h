#pragma once

#include <Core\Tools\Common.h>
#include <Core\Modules\Module.h>
#include <Core\Foundation\ProfilerModule.h>

namespace Core
{
	class ComputeModule : public Module
	{
	protected:
		typedef ComputeModule base;

	public:
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override { profilerModule = ExecuteAfter<ProfilerModule>(moduleManager); }

	protected:
		ProfilerModule* profilerModule;
	};
}