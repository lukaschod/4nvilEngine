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

#pragma once

#include <vector>
#include <thread>
#include <Core/Tools/Common.hpp>
#include <Core/Tools/Threading/AutoResetEvent.hpp>
#include <Core/Modules/IModuleExecutor.hpp>
#include <Core/Modules/Module.hpp>

namespace Core
{
    class ConcurrentModuleExecutor;

    class ConcurrentModuleWorker
    {
    public:
        ConcurrentModuleWorker(UInt32 index, ConcurrentModuleExecutor* executor, IModulePlanner* planner);
        ~ConcurrentModuleWorker();
        Void Reset();
        Void Start();
        Void Stop();
        Bool IsSleeping() const { return isSleeping; }

    private:
        Void Run();
        Void Sleep();
        Void Wakeup();

    private:
        IModulePlanner* planner;
        Threading::AutoResetEvent event;
        std::thread* thread;
        Bool isRunning;
        UInt32 cyclesBeforeSleep;
        UInt32 index;
        UInt64 executionIndex;
        Bool isSleeping;
    };

    class ConcurrentModuleExecutor : public IModuleExecutor
    {
    public:
        CORE_API ConcurrentModuleExecutor(IModulePlanner* planner, UInt32 workerCount = 1);
        CORE_API ~ConcurrentModuleExecutor();
        virtual Void Reset() override;
        virtual Void Start() override;
        virtual Void Stop() override;
        virtual Bool IsRunning() { return isRunning; };
        virtual UInt32 GetWorkerCount() override { return (UInt32) workers.size(); }

    private:
        List<ConcurrentModuleWorker*> workers;
        IModulePlanner* planner;
        Bool isRunning;
    };
}