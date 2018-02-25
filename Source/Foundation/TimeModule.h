#pragma once

#include <Tools\Common.h>
#include <Tools\StopWatch.h>
#include <Foundation\ComputeModule.h>

namespace Core
{
	class TimeModule : public ComputeModule
	{
	public:
		TimeModule();
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		inline uint64_t GetDeltaTimeMs() const { return deltaTimeMs; }
		inline float GetDeltaTime() const { return deltaTime; }

	private:
		StopWatch stopWatch;
		uint64_t deltaTimeMs;
		float deltaTime;
	};
}