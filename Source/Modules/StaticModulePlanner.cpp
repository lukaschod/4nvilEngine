#include <Modules\StaticModulePlanner.h>
#include <Modules\Module.h>
#include <Common\Collections\List.h>

class StaticModulePlanNode
{
public:
	List<StaticModulePlanNode*> childs;
	Module* module;
	uint32_t concunrency;
	uint32_t dependencies;

	StaticModulePlanNode(Module* module) :
		module(module),
		concunrency(0),
		dependencies(0)
	{ }

	StaticModulePlanNode* TryFindNode(Module* module)
	{
		if (this->module == module)
			return this;

		for (auto child : childs)
		{
			auto node = child->TryFindNode(module);
			if (node != nullptr)
				return node;
		}
		return nullptr;
	}
};

class StaticModulePlan
{
public:
	StaticModulePlanNode* root;
	List<StaticModulePlanNode*> nodes;

	StaticModulePlan(List<Module*>& modules)
	{
		root = new StaticModulePlanNode(nullptr);

		nodes.reserve(modules.size());
		for (auto module : modules)
			nodes.push_back(new StaticModulePlanNode(module));

		size_t addedCount = 0;
		while (addedCount != nodes.size())
		{
			for (auto node : nodes)
			{
				if (TryAdd(node))
					addedCount++;
			}
		}
	}

	inline bool TryAdd(StaticModulePlanNode* node)
	{
		// Check if it is not already in plan
		auto module = node->module;
		if (TryFindNode(module) != nullptr)
			return false;
		
		auto& dependencies = module->Get_dependencies();

		// If thre is no dependencies we can freely add it to root
		if (dependencies.empty())
			root->childs.push_back(node);

		// Check if all dependencies already in plan, if not we can't add it
		for (auto dependancy : dependencies)
		{
			auto dependancyNode = root->TryFindNode(dependancy);
			if (dependancyNode == nullptr)
				return false;
		}

		// Include to all dependencies this module as continue
		for (auto dependancy : dependencies)
		{
			auto dependancyNode = root->TryFindNode(dependancy);
			dependancyNode->childs.push_back(node);
		}

		return true;
	}

	inline StaticModulePlanNode* TryFindNode(Module* module)
	{
		return root->TryFindNode(module);
	}

	inline void Reset()
	{
		for (auto node : nodes)
		{
			node->dependencies = node->module->Get_dependencies().size();
			node->concunrency = 1;
		}
	}
};

StaticModulePlanner::StaticModulePlanner()
{

}

StaticModulePlanner::~StaticModulePlanner()
{
	SAFE_DELETE(plan);
}

void StaticModulePlanner::Recreate(List<Module*>& modules)
{
	std::lock_guard<std::mutex> lock(readyModulesMutex);
	SAFE_DELETE(plan);
	plan = new StaticModulePlan(modules);
}

void StaticModulePlanner::Reset()
{
	std::lock_guard<std::mutex> lock(readyModulesMutex);
	plan->Reset(); // Reset dependencies
	for (auto child : plan->root->childs)
	{
		ModuleJob job;
		job.module = child->module;
		job.offset = 0;
		job.size = job.module->GetExecutionkSize();
		job.userData = child;
		readyJobs.push(job);
	}
	jobExecutingCount = 0;
}

ModuleJob StaticModulePlanner::TryGetNext()
{
	std::lock_guard<std::mutex> lock(readyModulesMutex);
	if (readyJobs.empty())
		return ModuleJob();

	// Here we split big jobs
	auto job = readyJobs.front();

	// Check if need to split the job into multiple ones
	auto node = (StaticModulePlanNode*) job.userData;
	
	auto module = job.module;
	auto splitTreshold = module->GetSplitExecutionSize(job.size);
	if (splitTreshold != job.size)
	{
		auto& splitedJob = readyJobs.front();
		splitedJob.offset += (uint32_t)splitTreshold;
		splitedJob.size -= splitTreshold;
		job.size = splitTreshold;
		node->concunrency++;
	}
	else
	{
		readyJobs.pop();
	}

	jobExecutingCount++;
	return job;
}

void StaticModulePlanner::SetFinished(ModuleJob job)
{
	auto module = job.module;
	ASSERT(module != nullptr);

	std::lock_guard<std::mutex> lock(readyModulesMutex);
	//auto node = plan->TryFindNode(module); // TODO: Maybe we can optimize this without sacrificing design
	auto node = (StaticModulePlanNode*)job.userData; // How about this?
	ASSERT(node != nullptr);

	// Check if all job shards are completed
	if (--node->concunrency != 0)
	{
		jobExecutingCount--;
		return;
	}

	for (auto child : node->childs)
	{
		// If its not the last dependancy, skip it. It means it still depends on other modules
		ASSERT_MSG(child->dependencies != 0, "Something must corrupted very hard");
		if (--child->dependencies != 0)
			continue;

		ModuleJob childJob;
		childJob.module = child->module;
		childJob.offset = 0;
		childJob.size = childJob.module->GetExecutionkSize();
		childJob.userData = child;
		readyJobs.push(childJob);
	}

	// Notify that all jobs are finished
	jobExecutingCount--;
	if (finishCallback && readyJobs.size() == 0 && jobExecutingCount == 0)
		finishCallback();

	// Notify about finished job
	if (jobFinishCallback != nullptr)
		jobFinishCallback(readyJobs.size());
}