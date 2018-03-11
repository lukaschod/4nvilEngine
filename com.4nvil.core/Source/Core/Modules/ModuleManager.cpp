#include <algorithm>
#include <Core\Modules\ModuleManager.h>
#include <Core\Modules\Module.h>

using namespace Core;

ModuleManager::ModuleManager(IModulePlanner* planner, IModuleExecutor* executor)
	: planner(planner)
	, executor(executor)
	, requestedStop(false)
{
	ASSERT(planner != nullptr);
	ASSERT(executor != nullptr);

	planner->Set_finishCallback([this]()
	{
		sleepEvent.Set();
	});
}

ModuleManager::~ModuleManager()
{
	for (auto module : modules)
		delete module;
}

void ModuleManager::Start()
{
	for (size_t i = 0; i < modules.size(); i++)
		modules[i]->SetupExecuteOrder(this);
	planner->Recreate(modules);
	executor->Start();
}

void ModuleManager::Stop()
{
	executor->Stop();
}

void ModuleManager::NewFrame()
{
	planner->Reset();
	executor->Reset();
}

void ModuleManager::WaitForFrame()
{
	sleepEvent.WaitOne();
}

void ModuleManager::AddModule(Module* module)
{
	modules.push_back(module);
}