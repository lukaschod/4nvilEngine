#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Tools\StopWatch.hpp>
#include <Core\Foundation\ComputeModule.hpp>

namespace Core
{
	class TimeModule : public ComputeModule
	{
	public:
		BASE_IS(ComputeModule);

		TimeModule();
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		inline uint64 GetDeltaTimeMs() const { return deltaTimeMs; }
		inline float GetDeltaTime() const { return deltaTime; }

	private:
		StopWatch stopWatch;
		uint64 deltaTimeMs;
		float deltaTime;
	};
}