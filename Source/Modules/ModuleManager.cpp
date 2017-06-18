#include <Modules\ModuleManager.h>
#include <algorithm>

ModuleManager::ModuleManager(IModulePlanner* planner, IModuleExecuter* executer)
{
	DebugAssert(planner != nullptr);
	DebugAssert(executer != nullptr);
	this->planner = planner;
	this->executer = executer;

	planner->Set_finishCallback([this]()
	{
		sleepEvent.Set();
	});
}

ModuleManager::~ModuleManager()
{
	FOR_EACH(modules, itr)
	{
		auto module = *itr;
		delete module;
	}
}

void ModuleManager::Start()
{
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