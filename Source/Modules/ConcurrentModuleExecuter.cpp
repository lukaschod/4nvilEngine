#include <Modules\ModuleManager.h>
#include <algorithm>
#include "ConcurrentModuleExecuter.h"

ConcurrentModuleExecuter::ConcurrentModuleExecuter(IModulePlanner* planner, uint32_t workerCount) :
	planner(planner),
	isRunning(false)
{
	FOR_INC(workerCount, i)
	{
		auto worker = new ConcurrentModuleWorker(this, planner);
		workers.push_back(worker);
	}

	planner->Set_jobFinishCallback([this](uint32_t readyJobCount)
	{
		// Check if we need to wakeup some workers
		FOR_EACH(workers, itr)
		{
			if (readyJobCount == 0)
				return;

			auto worker = *itr;
			if (worker->Get_isSleeping())
				worker->Reset();
			readyJobCount--;
		}
	});
}

ConcurrentModuleExecuter::~ConcurrentModuleExecuter()
{
	SAFE_VECTOR_DELETE(workers);
}

void ConcurrentModuleExecuter::Reset()
{
	FOR_EACH(workers, itr)
	{
		auto worker = *itr;
		worker->Reset();
	}
}

void ConcurrentModuleExecuter::Start()
{
	isRunning = true;
	FOR_EACH(workers, itr)
	{
		auto worker = *itr;
		worker->Start();
	}
}

void ConcurrentModuleExecuter::Stop()
{
	isRunning = false;
	FOR_EACH(workers, itr)
	{
		auto worker = *itr;
		worker->Stop();
	}
}

ConcurrentModuleWorker::ConcurrentModuleWorker(ConcurrentModuleExecuter* executer, IModulePlanner* planner) :
	planner(planner),
	thread(nullptr),
	isRunning(false),
	isSleeping(false)
{
	DebugAssert(executer != nullptr);
	DebugAssert(planner != nullptr);
}

ConcurrentModuleWorker::~ConcurrentModuleWorker()
{
	SAFE_DELETE(thread);
}

void ConcurrentModuleWorker::Reset()
{
	Wakeup();
}

void ConcurrentModuleWorker::Start()
{
	DebugAssert(!isRunning);
	SAFE_DELETE(thread);
	isRunning = true;
	thread = new std::thread(&ConcurrentModuleWorker::Run, this);
}

void ConcurrentModuleWorker::Stop()
{
	DebugAssert(isRunning);
	isRunning = false;
}

void ConcurrentModuleWorker::Run()
{
	while (isRunning)
	{
		// Try to get next job
		auto job = planner->TryGetNext();
		auto module = job.module;
		if (module == nullptr)
		{
			// Sleep if no work is available
			Sleep();
			continue;
		}

		// Execute the job here
		module->Execute(job.offset, job.size);

		// Notify planner that job is finished
		planner->SetFinished(job);
	}
}

void ConcurrentModuleWorker::Sleep()
{
	isSleeping = true;
	event.WaitOne();
}

void ConcurrentModuleWorker::Wakeup()
{
	event.Set();
	isSleeping = false;
}
