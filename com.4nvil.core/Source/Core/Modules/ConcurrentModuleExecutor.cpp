/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <algorithm>
#include <Core/Modules/ModuleManager.hpp>
#include <Core/Modules/ConcurrentModuleExecutor.hpp>

using namespace Core;

ConcurrentModuleExecutor::ConcurrentModuleExecutor(IModulePlanner* planner, uint32 workerCount) 
    : planner(planner)
    , isRunning(false)
{
    for (uint32 i = 0; i < workerCount; i++)
    {
        auto worker = new ConcurrentModuleWorker(i, this, planner);
        workers.push_back(worker);
    }

    planner->Set_jobFinishCallback([this](size_t readyJobCount)
    {
        // Check if we need to wakeup some workers
        for (auto worker : workers)
        {
            if (readyJobCount == 0)
                return;

            if (worker->Get_isSleeping())
                worker->Reset();
            readyJobCount--;
        }
    });
}

ConcurrentModuleExecutor::~ConcurrentModuleExecutor()
{
    for (auto worker : workers)
        delete worker;
}

void ConcurrentModuleExecutor::Reset()
{
    for (auto worker : workers)
        worker->Reset();
}

void ConcurrentModuleExecutor::Start()
{
    isRunning = true;
    for (auto worker : workers)
        worker->Start();
}

void ConcurrentModuleExecutor::Stop()
{
    isRunning = false;
    for (auto worker : workers)
        worker->Stop();
}

ConcurrentModuleWorker::ConcurrentModuleWorker(uint32 index, ConcurrentModuleExecutor* executor, IModulePlanner* planner) 
    : planner(planner)
    , thread(nullptr)
    , isRunning(false)
    , isSleeping(false)
    , index(index)
    , executionIndex(0)
{
    ASSERT(executor != nullptr);
    ASSERT(planner != nullptr);
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
    ASSERT(!isRunning);
    SAFE_DELETE(thread);
    isRunning = true;
    thread = new std::thread(&ConcurrentModuleWorker::Run, this);
}

void ConcurrentModuleWorker::Stop()
{
    ASSERT(isRunning);
    isRunning = false;
    if (isSleeping)
        Wakeup();
    thread->join();
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
            if (cyclesBeforeSleep == 0)
            {
                // Sleep if no work is available
                Sleep();
            }
            else
                cyclesBeforeSleep--;
            
            continue;
        }
        cyclesBeforeSleep = 5; // TODO: Maybe we can find something more legit, instead of this magic number?

        ExecutionContext context;
        context.workerIndex = index;
        context.executingModule = module;
        context.start = job.offset;
        context.end = job.offset + (uint32)job.size;

        // Execute the job here
        module->Execute(context);
        executionIndex++;

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
