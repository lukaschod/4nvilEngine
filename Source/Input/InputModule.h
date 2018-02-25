#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>

namespace Core
{
	class InputModule : public PipeModule
	{
	public:
		virtual void SetupExecutionOrder(ModuleManager* manager) override;
	};
}