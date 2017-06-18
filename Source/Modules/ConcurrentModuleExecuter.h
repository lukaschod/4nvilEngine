#pragma once

#include <Common\EngineCommon.h>
#include <Common\AutoResetEvent.h>
#include <Modules\IModuleExecuter.h>
#include <Modules\Module.h>
#include <vector>
#include <thread>

class ConcurrentModuleExecuter;

class ConcurrentModuleWorker
{
public:
	ConcurrentModuleWorker(ConcurrentModuleExecuter* executer, IModulePlanner* planner);
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
	AUTOMATED_PROPERTY_GET(bool, isSleeping);
};

class ConcurrentModuleExecuter : public IModuleExecuter
{
public:
	ConcurrentModuleExecuter(IModulePlanner* planner, uint32_t workerCount = 1);
	~ConcurrentModuleExecuter();
	virtual void Reset() override;
	virtual void Start() override;
	virtual void Stop() override;
	virtual bool IsRunning(){ return isRunning; };

private:
	std::vector<ConcurrentModuleWorker*> workers;
	IModulePlanner* planner;
	bool isRunning;
};