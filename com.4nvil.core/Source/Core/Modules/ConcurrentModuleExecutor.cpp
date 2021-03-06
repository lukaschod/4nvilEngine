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

ConcurrentModuleExecutor::ConcurrentModuleExecutor(IModulePlanner* planner, UInt32 workerCount) 
    : planner(planner)
    , isRunning(false)
{
    for (UInt32 i = 0; i < workerCount; i++)
    {
        auto worker = new ConcurrentModuleWorker(i, this, planner);
        workers.push_back(worker);
    }

    planner->SetJobFinishCallback([this](UInt readyJobCount)
    {
        // Check if we need to wakeup some workers
        for (auto worker : workers)
        {
            if (readyJobCount == 0)
                return;

            if (worker->IsSleeping())
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

Void ConcurrentModuleExecutor::Reset()
{
    for (auto worker : workers)
        worker->Reset();
}

Void ConcurrentModuleExecutor::Start()
{
    isRunning = true;
    for (auto worker : workers)
        worker->Start();
}

Void ConcurrentModuleExecutor::Stop()
{
    isRunning = false;
    for (auto worker : workers)
        worker->Stop();
}

ConcurrentModuleWorker::ConcurrentModuleWorker(UInt32 index, ConcurrentModuleExecutor* executor, IModulePlanner* planner) 
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
    if (thread != nullptr)
        delete thread;
}

Void ConcurrentModuleWorker::Reset()
{
    Wakeup();
}

Void ConcurrentModuleWorker::Start()
{
    ASSERT(!isRunning);
    if (thread != nullptr)
        delete thread;
    isRunning = true;
    thread = new std::thread(&ConcurrentModuleWorker::Run, this);
}

Void ConcurrentModuleWorker::Stop()
{
    ASSERT(isRunning);
    isRunning = false;
    if (isSleeping)
        Wakeup();
    thread->join();
}

Void ConcurrentModuleWorker::Run()
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
        context.end = job.offset + (UInt32)job.size;

        // Execute the job here
        module->Execute(context);
        executionIndex++;

        // Notify planner that job is finished
        planner->SetFinished(job);
    }
}

Void ConcurrentModuleWorker::Sleep()
{
    isSleeping = true;
    event.WaitOne();
}

Void ConcurrentModuleWorker::Wakeup()
{
    event.Set();
    isSleeping = false;
}
