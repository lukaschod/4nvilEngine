#pragma once

#include <Tools\Common.h>
#include <Tools\Threading\AutoResetEvent.h>
#include <Modules\IModuleExecutor.h>
#include <Modules\Module.h>
#include <vector>
#include <thread>

class ConcurrentModuleExecutor;

class ConcurrentModuleWorker
{
public:
	ConcurrentModuleWorker(uint32_t index, ConcurrentModuleExecutor* executor, IModulePlanner* planner);
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
	AutoResetEvent event;
	std::thread* thread;
	bool isRunning;
	uint32_t cyclesBeforeSleep;
	uint32_t index;
	uint64_t executionIndex;
	AUTOMATED_PROPERTY_GET(bool, isSleeping);
};

class ConcurrentModuleExecutor : public IModuleExecutor
{
public:
	ConcurrentModuleExecutor(IModulePlanner* planner, uint32_t workerCount = 1);
	~ConcurrentModuleExecutor();
	virtual void Reset() override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual bool IsRunning(){ return isRunning; };
	virtual uint32_t GetWorkerCount() override { return (uint32_t)workers.size(); }

private:
	List<ConcurrentModuleWorker*> workers;
	IModulePlanner* planner;
	bool isRunning;
};