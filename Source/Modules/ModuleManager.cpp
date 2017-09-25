#include <Modules\ModuleManager.h>
#include <Modules\Module.h>
#include <algorithm>

ModuleManager::ModuleManager(IModulePlanner* planner, IModuleExecuter* executer)
{
	ASSERT(planner != nullptr);
	ASSERT(executer != nullptr);
	this->planner = planner;
	this->executer = executer;

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
	executer->Start();
}

void ModuleManager::Stop()
{
	executer->Stop();
}

void ModuleManager::NewFrame()
{
	planner->Reset();
	executer->Reset();
}

void ModuleManager::WaitForFrame()
{
	sleepEvent.WaitOne();
}

void ModuleManager::AddModule(Module* module)
{
	modules.push_back(module);
}