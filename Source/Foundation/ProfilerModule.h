#pragma once

#include <Tools\Common.h>
#include <Tools\StopWatch.h>
#include <Modules\Module.h>

namespace Core
{
	struct ProfiledFunction
	{
		ProfiledFunction() {}
		ProfiledFunction(
			const char* name,
			float start,
			int parentIndex
		)
			: name(name)
			, start(start)
			, end(start)
			, childFunctionCount(0)
			, parentIndex(parentIndex)
		{
		}
		const char* name;
		float start;
		float end;
		size_t childFunctionCount;
		int parentIndex;
	};

	struct ProfiledWorker
	{
		uint32_t workerIndex;
		List<ProfiledFunction> functions;
	};

	class LogModule;

	class ProfilerModule : public Module
	{
	public:
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;

		void TraceFunctions();

	public:
		void RecPushFunction(const ExecutionContext& context, const char* name);
		void RecPopFunction(const ExecutionContext& context);

	private:
		LogModule* logModule;
		List<ProfiledWorker> workers;
		StopWatch stopWatch;
		uint64_t frameLeftUntilProfile;
	};

#if !defined(ENABLED_MARK_FUNCTION) && defined(ENABLED_DEBUG)
#	define ENABLED_MARK_FUNCTION
#endif

#ifdef ENABLED_MARK_FUNCTION
#	define MARK_FUNCTION ProfileFunction profileFunction(context, profilerModule, __FUNCTION__)
#else
#	define MARK_FUNCTION (void) 0
#endif

	struct ProfileFunction
	{
	public:
		ProfileFunction(
			const ExecutionContext& context,
			ProfilerModule* profiler,
			const char* name)
			: profiler(profiler)
			, context(context)
		{
			profiler->RecPushFunction(context, name);
		}

		~ProfileFunction()
		{
			profiler->RecPopFunction(context);
		}

	private:
		const ExecutionContext& context;
		ProfilerModule* profiler;
	};
}