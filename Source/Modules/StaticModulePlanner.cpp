#include <Modules\StaticModulePlanner.h>

class StaticModulePlanNode
{
public:
	std::vector<StaticModulePlanNode> childs;
	Module* module = nullptr;
	uint32_t depth = 0;

	inline void Add(Module* module)
	{
		StaticModulePlanNode node;
		node.depth = depth + 1;
		node.module = module;
		childs.push_back(node);
	}

	StaticModulePlanNode* TryFindNode(Module* module)
	{
		if (this->module == module)
			return this;

		FOR_EACH(childs, itr)
		{
			auto& child = *itr;
			auto node = child.TryFindNode(module);
			if (node != nullptr)
				return node;
		}
		return nullptr;
	}
};

class StaticModulePlan
{
public:
	StaticModulePlanNode root;

	StaticModulePlan(std::vector<Module*>& modules)
	{
		auto addedCount = 0;
		while (addedCount != modules.size())
		{
			FOR_EACH(modules, itr)
			{
				auto module = *itr;
				if (TryFindNode(module) == nullptr && TryAdd(module))
				{
					addedCount++;
				}
			}
		}
	}

	inline bool TryAdd(Module* module)
	{
		DebugAssert(module != nullptr);
		StaticModulePlanNode* maxDepthNode = &root;
		auto& dependencies = module->Get_dependencies();
		FOR_EACH(dependencies, itr)
		{
			auto dependancy = *itr;
			auto node = root.TryFindNode(dependancy);
			if (node == nullptr)
				return false;
			
			if (maxDepthNode->depth < node->depth)
			{
				maxDepthNode = node;
			}
		}
		maxDepthNode->Add(module);
		return true;
	}

	inline StaticModulePlanNode* TryFindNode(Module* module)
	{
		return root.TryFindNode(module);
	}
};

StaticModulePlanner::StaticModulePlanner()
{

}

StaticModulePlanner::~StaticModulePlanner()
{
	SAFE_DELETE(plan);
}

void StaticModulePlanner::Recreate(std::vector<Module*>& modules)
{
	std::lock_guard<std::mutex> lock(readyModulesMutex);
	SAFE_DELETE(plan);
	plan = new StaticModulePlan(modules);
}

void StaticModulePlanner::Reset()
{
	std::lock_guard<std::mutex> lock(readyModulesMutex);
	FOR_EACH(plan->root.childs, itr)
	{
		auto& child = *itr;
		ModuleJob job;
		job.module = child.module;
		job.offset = 0;
		job.size = job.module->GetExecutionkSize();
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
	auto module = job.module;
	auto splitTreshold = module->GetSplitExecutionTreshold();
	if (job.size > splitTreshold)
	{
		auto& splitedJob = readyJobs.front();
		splitedJob.offset += splitTreshold;
		splitedJob.size -= splitTreshold;
		job.size = splitTreshold;
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
	DebugAssert(module != nullptr);

	// This means that this job is not final for module, so we don't add childs until that
	if (module->GetSplitExecutionTreshold() < job.size)
		return;

	std::lock_guard<std::mutex> lock(readyModulesMutex);
	auto node = plan->TryFindNode(module); // TODO: Maybe we can optimize this without sacrificing design
	DebugAssert(node != nullptr);
	FOR_EACH(node->childs, itr)
	{
		auto& child = *itr;
		ModuleJob job;
		job.module = child.module;
		job.offset = 0;
		job.size = job.module->GetExecutionkSize();
		readyJobs.push(job);
	}

	// Notify that all jobs are finished
	jobExecutingCount--;
	if (finishCallback && readyJobs.size() == 0 && jobExecutingCount == 0)
		finishCallback();

	// Notify about finished job
	if (jobFinishCallback != nullptr)
		jobFinishCallback(readyJobs.size());
}