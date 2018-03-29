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
#include <Core\Tools\Common.hpp>
#include <Core\Tools\Threading\AutoResetEvent.hpp>
#include <Core\Modules\IModuleExecutor.hpp>
#include <Core\Modules\Module.hpp>

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