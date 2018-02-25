#pragma once

#include <Tools\Common.h>
#include <Modules\Module.h>
#include <Foundation\ProfilerModule.h>

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