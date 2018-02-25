#pragma once

#include <vector>
#include <thread>
#include <Tools\Common.h>
#include <Tools\Threading\AutoResetEvent.h>
#include <Modules\IModuleExecutor.h>
#include <Modules\Module.h>

namespace Core
{
	class ConcurrentModuleExecutor;

	class ConcurrentModuleWorker
	{
	public:
		ConcurrentModuleWorker(uint32 index, ConcurrentModuleExecutor* executor, IModulePlanner* planner);
		~ConcurrentModuleWorker();
		void Reset();
		void Start();
		void Stop();

	private:
		void Run();
		void Sleep();
		void Wakeup();

	private:
		IModulePlanner* planner;
		Threading::AutoResetEvent event;
		std::thread* thread;
		bool isRunning;
		uint32 cyclesBeforeSleep;
		uint32 index;
		uint64 executionIndex;
		AUTOMATED_PROPERTY_GET(bool, isSleeping);
	};

	class ConcurrentModuleExecutor : public IModuleExecutor
	{
	public:
		ConcurrentModuleExecutor(IModulePlanner* planner, uint32 workerCount = 1);
		~ConcurrentModuleExecutor();
		virtual void Reset() override;
		virtual void Start() override;
		virtual void Stop() override;
		virtual bool IsRunning() { return isRunning; };
		virtual uint32 GetWorkerCount() override { return (uint32) workers.size(); }

	private:
		List<ConcurrentModuleWorker*> workers;
		IModulePlanner* planner;
		bool isRunning;
	};
}