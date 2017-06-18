#include <Modules\ModuleManager.h>
#include <Modules\ConcurrentModuleExecuter.h>
#include <Modules\StaticModulePlanner.h>

#include <Graphics\GraphicsModule.h>
#include <Transforms\TransformsModule.h>

int main()
{
	auto planner = new StaticModulePlanner();
	auto executer = new ConcurrentModuleExecuter(planner);
	auto moduleManager = new ModuleManager(planner, executer);

	auto graphics = new GraphicsModule(4);
	moduleManager->AddModule(graphics);

	auto transforms = new TransformsModule();
	transforms->AddDependancy(graphics);
	moduleManager->AddModule(transforms);

	auto transforms2 = new TransformsModule();
	transforms2->AddDependancy(graphics);
	moduleManager->AddModule(transforms2);

	moduleManager->Start();

	while (moduleManager->IsRunning())
	{
		printf("New Frame\n");
		moduleManager->NewFrame();
		moduleManager->WaitForFrame();
	}

	moduleManager->Stop();

	delete moduleManager;
	delete executer;
	delete planner;

	return 0;
}