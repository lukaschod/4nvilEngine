#include <Modules\StaticModulePlanner.h>
#include <Modules\Module.h>
#include <Tools\Collections\List.h>
#include <Tools\Math\Math.h>

using namespace Core;

StaticModulePlanNode::StaticModulePlanNode(Module* module)
	: module(module)
	, concunrency(0)
	, dependencies(0)
{
}

StaticModulePlanNode* StaticModulePlanNode::TryFindNode(Module* module)
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

StaticModulePlan::StaticModulePlan(List<Module*>& modules)
{
	root = new StaticModulePlanNode(nullptr);

	nodes.reserve(modules.size());
	for (auto module : modules)
		nodes.push_back(new StaticModulePlanNode(module));

	// TODO: Validate the plan, etc. cycles

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

bool StaticModulePlan::TryAdd(StaticModulePlanNode* node)
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

StaticModulePlanNode* StaticModulePlan::TryFindNode(Module* module)
{
	return root->TryFindNode(module);
}

void StaticModulePlan::Reset()
{
	for (auto node : nodes)
	{
		node->dependencies = (uint32_t) node->module->Get_dependencies().size();
	}
}

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
	for (auto child : plan->GetRoot()->childs)
	{
		AddJob(child);
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
	readyJobs.pop();

	jobExecutingCount++;
	return job;
}

void StaticModulePlanner::SetFinished(const ModuleJob& job)
{
	auto module = job.module;
	ASSERT(module != nullptr);

	std::lock_guard<std::mutex> lock(readyModulesMutex);
	//auto node = plan->TryFindNode(module); // TODO: Maybe we can optimize this without sacrificing design
	auto node = (StaticModulePlanNode*) job.userData; // How about this?
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

		AddJob(child);
	}

	// Notify that all jobs are finished
	jobExecutingCount--;
	if (finishCallback && readyJobs.size() == 0 && jobExecutingCount == 0)
		finishCallback();

	// Notify about finished job
	if (jobFinishCallback != nullptr)
		jobFinishCallback(readyJobs.size());
}

void StaticModulePlanner::AddJob(StaticModulePlanNode* node)
{
	// TODO: Doesn't look that clean as it could be, maybe we can do better?

	node->concunrency = 0;

	auto jobSize = node->module->GetExecutionSize();
	if (jobSize == 0)
	{
		ModuleJob childJob;
		childJob.module = node->module;
		childJob.offset = 0;
		childJob.size = jobSize;
		childJob.userData = node;
		readyJobs.push(childJob);
		node->concunrency++;
		return;
	}

	auto offset = 0;
	while (jobSize != 0)
	{
		auto split = node->module->GetSplitExecutionSize();
		split = Math::Clamp(split, (size_t) 1, jobSize);

		ModuleJob childJob;
		childJob.module = node->module;
		childJob.offset = offset;
		childJob.size = split;
		childJob.userData = node;
		readyJobs.push(childJob);
		node->concunrency++;

		offset += (uint32_t) split;
		jobSize -= (uint32_t) split;
	}
}