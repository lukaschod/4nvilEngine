#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <Tools\Common.h>
#include <Modules\IModulePlanner.h>

namespace Core
{
	struct StaticModulePlanNode
	{
		StaticModulePlanNode(Module* module);
		StaticModulePlanNode* TryFindNode(Module* module);

		List<StaticModulePlanNode*> childs;
		Module* module;
		uint32_t concunrency;
		uint32_t dependencies;
	};

	class StaticModulePlan
	{
	public:
		StaticModulePlan(List<Module*>& modules);
		inline bool TryAdd(StaticModulePlanNode* node);
		inline StaticModulePlanNode* TryFindNode(Module* module);
		inline void Reset();

		inline StaticModulePlanNode* GetRoot() const { return root; }

	private:
		StaticModulePlanNode* root;
		List<StaticModulePlanNode*> nodes;
	};

	class StaticModulePlanner : public IModulePlanner
	{
	public:
		StaticModulePlanner();
		~StaticModulePlanner();
		virtual void Recreate(List<Module*>& modules) override;
		virtual void Reset() override;
		virtual ModuleJob TryGetNext() override;
		virtual void SetFinished(const ModuleJob& module) override;

	private:
		void AddJob(StaticModulePlanNode* node);

	private:
		StaticModulePlan* plan;
		std::queue<ModuleJob> readyJobs;
		std::mutex readyModulesMutex;
		size_t jobExecutingCount;
	};
}